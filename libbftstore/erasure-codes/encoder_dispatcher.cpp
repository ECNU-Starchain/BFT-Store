#include "encoder_internal.h"

namespace erasure
{
	void matrix_mul(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes)
	{
		matrix_mul_avx2(mat, inputs, outputs, n_inputs, n_outputs, num_bytes);
	}
}
