#include "stream_api.h"
#include "encoder_internal.h"

#include <numeric>
#include <new>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <alloca.h>
#endif

struct erasure_recover_stream_
{
	erasure::matrix decode;
	uint8_t* in_indices;
	uint8_t* out_indices;
	void* buffer; // buffer for in_indices and out_indices
	erasure::matrix_mul_proc mul_proc;
	size_t data_size;
	uint8_t n_inputs;
	uint8_t n_outputs;
	uint8_t n_shards;
};

namespace erasure
{
#ifdef ERASURE_NO_ALLOCA
#	define stackalloc(size) malloc(size)
#	define stackfree(ptr) free(ptr)
#else
#	define stackalloc(size) alloca(size)
#	define stackfree(ptr);
	// Use this to disable returned pointer checks
	// for alloca since it should never be null in
	// cases other than n_data == n_shards
#	define STACKALLOC_IS_ALLOCA
#endif
	
	recover_stream* create_recover_stream(
		rs_encoder* encoder,
		const bool* present)
	{
		if (!encoder || !present)
			return nullptr;

		uint8_t n_data =
			std::accumulate(
				present,
				present + encoder->n_data,
				uint8_t(0));

		uint8_t n_present =
			std::accumulate(
				present + encoder->n_data,
				present + encoder->n_shards,
				n_data);

		if (n_present < encoder->n_data)
			return nullptr;

		void* buffer = malloc(sizeof(uint8_t) * encoder->n_shards);

		if (!buffer)
			return nullptr;

		uint8_t* inputs = (uint8_t*)buffer;
		uint8_t* outputs = (uint8_t*)buffer + encoder->n_data;

		matrix m = encoder->coding_mat;
		matrix decode = matrix{ encoder->n_data, encoder->n_data };

		uint8_t n_inputs = 0;
		for (uint8_t i = 0; i < encoder->n_shards && n_inputs < encoder->n_data; ++i)
		{
			if (present[i])
			{
				decode[n_inputs] = m[i];
				inputs[n_inputs] = i;
				++n_inputs;
			}
		}

		inverse(decode);

		uint8_t n_outputs = 0;
		for (uint8_t i = 0; i < encoder->n_data; ++i)
		{
			if (!present[i])
			{
				outputs[n_outputs] = i;
				decode[n_outputs] = decode[i];
				++n_outputs;
			}
		}

		decode = decode.submatrix(
			0, n_outputs,
			0, encoder->n_data);

		return new(std::nothrow) recover_stream{
			decode,
			inputs,
			outputs,
			buffer,
			encoder->mul_proc,
			encoder->data_size,
			n_inputs,
			n_outputs,
			encoder->n_shards
		};
	}
	void destroy_stream(recover_stream* stream)
	{
		free(stream->buffer);
		delete stream;
	}

	error_code stream_recover_data(
		recover_stream* stream,
		uint8_t* const* shards)
	{
		if (!stream || !shards)
			return INVALID_ARGUMENTS;

		for (size_t i = 0; i < stream->n_shards; ++i)
			if (!shards[i])
				return INVALID_ARGUMENTS;

		if (stream->n_outputs == 0)
			return SUCCESS;

		uint8_t** inputs = (uint8_t**)stackalloc(sizeof(uint8_t*) * stream->n_inputs);

#ifndef STACKALLOC_IS_ALLOCA
		// Check for allocation failure
		if (!inputs) return INTERNAL_ERROR;
#endif

		uint8_t** outputs = (uint8_t**)stackalloc(sizeof(uint8_t*) * stream->n_outputs);

#ifndef STACKALLOC_IS_ALLOCA
		// Check for allocation failure
		if (!outputs) return INTERNAL_ERROR;
#endif

		for (size_t i = 0; i < stream->n_inputs; ++i)
			inputs[i] = shards[stream->in_indices[i]];
		for (size_t i = 0; i < stream->n_outputs; ++i)
			outputs[i] = shards[stream->out_indices[i]];

		stream->mul_proc(
			stream->decode,
			inputs,
			outputs,
			stream->n_inputs,
			stream->n_outputs,
			stream->data_size);

		stackfree(inputs);
		stackfree(outputs);

		return SUCCESS;
	}
}