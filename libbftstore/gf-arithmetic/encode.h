#ifndef GFARITH_ENCODE_H
#define GFARITH_ENCODE_H

#include "detectcpu.h"
#include "matrix.h"

#include <cstdint>

namespace gfarith
{
	namespace adv
	{
		void mul_add_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
		void mul_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
	}

#if defined GFARITH_ARCH_X86_64 || defined GFARITH_ARCH_I386
	namespace ssse3
	{
		void mul_add_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
		void mul_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
	}

	namespace avx2
	{
		void mul_add_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
		void mul_row(uint8_t val, const uint8_t* in, uint8_t* out, size_t num_bytes);
	}
#endif

	typedef void(*matrix_mul_proc)(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		const size_t n_inputs,
		const size_t n_outputs,
		const size_t num_bytes);


	/*
	Preconditions:
	No alignment or size restrictions.
	*/
	void matrix_mul_basic(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	/*
	Preconditions:
	No alignment or size restrictions.
	*/
	void matrix_mul_adv(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	/*
	Preconditions:
	Inputs 16 byte aligned.
	*/
	void matrix_mul_sse(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	/*
	Preconditions:
	None
	*/
	void matrix_mul_avx2(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);
}

#endif
