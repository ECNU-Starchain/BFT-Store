#include "../encoder_internal.h"

namespace erasure
{
	void matrix_mul_adv(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes)
	{
		for (size_t i = 0; i < n_outputs; ++i)
			std::memset(outputs[i], 0, num_bytes);

		for (size_t r = 0; r < n_outputs; ++r)
		{
			uint8_t* out = outputs[r];

			for (size_t c = 0; c < n_inputs; ++c)
			{
				uint8_t val = mat(r, c).value;
				const uint8_t* in = inputs[c];
				if (c == 0)
					adv::mul_row(val, in, out, num_bytes);
				else
					adv::mul_add_row(val, in, out, num_bytes);
			}
		}
	}
}
