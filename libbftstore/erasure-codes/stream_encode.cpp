#include "stream_api.h"
#include "encoder_internal.h"

#include <numeric>
#include <new>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <alloca.h>
#endif

struct erasure_encode_stream_
{
	erasure::matrix mat;
	uint8_t* indices;
	erasure::matrix_mul_proc mul_proc;
	size_t data_size;
	uint8_t n_data;
	uint8_t n_outputs;
	uint8_t n_parity;
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
	
	encode_stream* create_encode_stream(
		rs_encoder* encoder,
		const bool* should_encode)
	{
		if (!encoder || !should_encode)
			return nullptr;

		uint8_t mat_sz = std::accumulate(should_encode,
			should_encode + encoder->n_parity, uint8_t(0));

		matrix mat{ mat_sz, encoder->n_data };

		uint8_t* outputs = new uint8_t[mat_sz];
		for (size_t i = 0, j = 0; i < encoder->n_parity && j < mat_sz; ++i)
		{
			if (should_encode[i])
			{
				mat[j] = encoder->coding_mat[i + encoder->n_data];

				outputs[j] = (uint8_t)i;
				++j;
			}
		}

		return new(std::nothrow) encode_stream{
			mat,
			outputs,
			encoder->mul_proc,
			encoder->data_size,
			encoder->n_data,
			mat_sz,
			encoder->n_parity
		};
	}
	void destroy_stream(encode_stream* stream)
	{
		delete[] stream->indices;
		delete stream;
	}

	error_code stream_encode(
		encode_stream* stream,
		const uint8_t* const* shards,
		uint8_t* const* parity)
	{
		if (!stream || !shards || !parity)
			return INVALID_ARGUMENTS;

		for (size_t i = 0; i < stream->n_data; ++i)
			if (!shards[i])
				return INVALID_ARGUMENTS;
		for (size_t i = 0; i < stream->n_parity; ++i)
			if (!parity[i])
				return INVALID_ARGUMENTS;

		if (stream->n_outputs == 0)
			return SUCCESS;

		uint8_t** outputs = (uint8_t**)stackalloc(sizeof(uint8_t*) * stream->n_outputs);

#ifndef STACKALLOC_IS_ALLOCA
		// Check for allocation failure
		if (!outputs) return INTERNAL_ERROR;
#endif

		for (size_t i = 0; i < stream->n_outputs; ++i)
		{
			outputs[i] = parity[stream->indices[i]];
		}

		stream->mul_proc(
			stream->mat,
			shards,
			outputs,
			stream->n_data,
			stream->n_outputs,
			stream->data_size);

		stackfree(outputs);

		return SUCCESS;
	}
}
