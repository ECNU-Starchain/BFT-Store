#ifndef ERASURE_ENCODER_INTERNAL_H
#define ERASURE_ENCODER_INTERNAL_H

#include "liberasure.h"
#include <encode.h>

#include <cstdint>
#include "matrix.h"

namespace erasure
{
	alignas(64) const extern uint8_t lohi_table[256][2][16];

	namespace adv = gfarith::adv;
	namespace ssse3 = gfarith::ssse3;
	namespace avx2 = gfarith::avx2;

	using gfarith::matrix_mul_basic;
	using gfarith::matrix_mul_adv;
	using gfarith::matrix_mul_sse;
	using gfarith::matrix_mul_avx2;

	using gfarith::matrix_mul_proc;

	/* Selects the fastest method based on
	   alignment of the input and output
	   pointers.
	*/
	void matrix_mul(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes);

	typedef erasure_encoder_ rs_encoder;
}

struct erasure_encoder_
{
	erasure::matrix coding_mat;
	size_t data_size;
	erasure::matrix_mul_proc mul_proc;
	uint8_t n_data;
	uint8_t n_shards;
	uint8_t n_parity;
};

#endif
