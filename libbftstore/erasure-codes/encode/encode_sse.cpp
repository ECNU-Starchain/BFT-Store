#include "../encoder_internal.h"

#include <tmmintrin.h>

namespace erasure
{
	namespace
	{
		constexpr size_t round_mask = ~(sizeof(__m128i) - 1);

		bool is_aligned(const uint8_t* ptr)
		{
			return ((std::uintptr_t)ptr & ~round_mask) == 0;
		}

	}

	void matrix_mul_sse(
		const matrix& mat,
		const uint8_t* const* inputs,
		uint8_t* const* outputs,
		size_t n_inputs,
		size_t n_outputs,
		size_t num_bytes)
	{
		for (size_t r = 0; r < n_outputs; ++r)
		{
			uint8_t* out = outputs[r];

			assert(is_aligned(out));

			for (size_t c = 0; c < n_inputs; ++c)
			{
				const uint8_t* in = inputs[c];

				assert(is_aligned(in));

				if (c == 0)
					ssse3::mul_row(mat(r, c).value, in, out, num_bytes & round_mask);
				else
					ssse3::mul_add_row(mat(r, c).value, in, out, num_bytes & round_mask);
			}
		}

		if ((num_bytes & ~round_mask) != 0)
		{
			for (size_t r = 0; r < n_outputs; ++r)
			{
				uint8_t* out = outputs[r] + (num_bytes & round_mask);

				for (size_t c = 0; c < n_outputs; ++r)
				{
					const uint8_t* in = inputs[c] + (num_bytes & round_mask);
					const auto val = mat(r, c).value;

					if (c == 0)
						adv::mul_row(mat(r, c).value, in, out, num_bytes & ~round_mask);
					else
						adv::mul_add_row(mat(r, c).value, in, out, num_bytes & ~round_mask);
				}
			}
		}
	}
}
/*
+		hi	{m128i_i8=0x000000b54d6ff8f0 "" m128i_i16=0x000000b54d6ff8f0 {0x2a00, 0x7e54, 0x82a8, 0xd6fc, 0x674d, ...} ...}	__m128i
[0x00000000]	0x00 '\0'	char
[0x00000001]	0x2a '*'	char
[0x00000002]	0x54 'T'	char
[0x00000003]	0x7e '~'	char
[0x00000004]	0xa8 '�'	char
[0x00000005]	0x82 '�'	char
[0x00000006]	0xfc '�'	char
[0x00000007]	0xd6 '�'	char
[0x00000008]	0x4d 'M'	char
[0x00000009]	0x67 'g'	char
[0x0000000a]	0x19 '\x19'	char
[0x0000000b]	0x33 '3'	char
[0x0000000c]	0xe5 '�'	char
[0x0000000d]	0xcf '�'	char
[0x0000000e]	0xb1 '�'	char
[0x0000000f]	0x9b '�'	char
-		lo	{m128i_i8=0x000000b54d6ff8d0 "" m128i_i16=0x000000b54d6ff8d0 {0x2100, 0x6342, 0xa584, 0xe7c6, 0x3415, ...} ...}	__m128i
[0x00000000]	0x00 '\0'	char
[0x00000001]	0x21 '!'	char
[0x00000002]	0x42 'B'	char
[0x00000003]	0x63 'c'	char
[0x00000004]	0x84 '�'	char
[0x00000005]	0xa5 '�'	char
[0x00000006]	0xc6 '�'	char
[0x00000007]	0xe7 '�'	char
[0x00000008]	0x15 '\x15'	char
[0x00000009]	0x34 '4'	char
[0x0000000a]	0x57 'W'	char
[0x0000000b]	0x76 'v'	char
[0x0000000c]	0x91 '�'	char
[0x0000000d]	0xb0 '�'	char
[0x0000000e]	0xd3 '�'	char
[0x0000000f]	0xf2 '�'	char
-		hi_vals	{m128i_i8=0x000000b54d6ff9c0 "����\x19*3~\x19�*��~�M... m128i_i16=0x000000b54d6ff9c0 {0xb1b1, 0x9b82, ...} ...}	__m128i
[0x00000000]	0xb1 '�'	char
[0x00000001]	0xb1 '�'	char
[0x00000002]	0x82 '�'	char
[0x00000003]	0x9b '�'	char
[0x00000004]	0x19 '\x19'	char
[0x00000005]	0x2a '*'	char
[0x00000006]	0x33 '3'	char
[0x00000007]	0x7e '~'	char
[0x00000008]	0x19 '\x19'	char
[0x00000009]	0x82 '�'	char
[0x0000000a]	0x2a '*'	char
[0x0000000b]	0x82 '�'	char
[0x0000000c]	0x82 '�'	char
[0x0000000d]	0x7e '~'	char
[0x0000000e]	0xcf '�'	char
[0x0000000f]	0x4d 'M'	char
-		lo_vals	{m128i_i8=0x000000b54d6ff9a0 "�4W!!c�WB\x154�v��... m128i_i16=0x000000b54d6ff9a0 {0x84e7, 0x5734, 0x2121, ...} ...}	__m128i
[0x00000000]	0xe7 '�'	char
[0x00000001]	0x84 '�'	char
[0x00000002]	0x34 '4'	char
[0x00000003]	0x57 'W'	char
[0x00000004]	0x21 '!'	char
[0x00000005]	0x21 '!'	char
[0x00000006]	0x63 'c'	char
[0x00000007]	0xe7 '�'	char
[0x00000008]	0x57 'W'	char
[0x00000009]	0x42 'B'	char
[0x0000000a]	0x15 '\x15'	char
[0x0000000b]	0x34 '4'	char
[0x0000000c]	0x91 '�'	char
[0x0000000d]	0x76 'v'	char
[0x0000000e]	0x91 '�'	char
[0x0000000f]	0xb0 '�'	char

*/
