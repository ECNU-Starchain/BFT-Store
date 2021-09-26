#include "../encoder_internal.h"

#include <immintrin.h>

namespace erasure
{
	namespace 
	{
		const size_t round_mask = ~(31);
	}

	void matrix_mul_avx2(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		const size_t n_inputs,
		const size_t n_outputs,
		const size_t num_bytes)
	{
		for (size_t r = 0; r < n_outputs; ++r)
		{
			uint8_t* out = outputs[r];

			for (size_t c = 0; c < n_inputs; ++c)
			{
				const uint8_t* in = inputs[c];

				if (c == 0)
					avx2::mul_row(mat(r, c).value, in, out, num_bytes & round_mask);
				else
					avx2::mul_add_row(mat(r, c).value, in, out, num_bytes & round_mask);
			}
		}
		
		if ((num_bytes & ~round_mask) != 0)
		{
			size_t completed = (num_bytes & round_mask);
			size_t remainder = (num_bytes & ~round_mask);

			for (size_t r = 0; r < n_outputs; ++r)
			{
				uint8_t* out = outputs[r] + completed;

				for (size_t c = 0; c < n_inputs; ++c)
				{
					const uint8_t* in = inputs[c] + completed;

					if (c == 0)
						adv::mul_row(mat(r, c).value, in, out, remainder);
					else
						adv::mul_add_row(mat(r, c).value, in, out, remainder);
				}
			}
		}
	}
}
