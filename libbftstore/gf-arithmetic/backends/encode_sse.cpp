#include "lohi_table.h"
#include "encode.h"

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>

#ifndef __SSSE3__
#error "SSSE3 arch support required"
#endif
#endif

namespace gfarith
{
	namespace ssse3
	{
		void mul_add_row(
			uint8_t val, 
			const uint8_t* in, 
			uint8_t* out, 
			size_t num_bytes)
		{
			assert(num_bytes % 16 == 0);

			const __m128i mask = _mm_set1_epi8(0x0F);

			__m128i lo = _mm_load_si128((const __m128i*)lohi_table[val][0]);
			__m128i hi = _mm_load_si128((const __m128i*)lohi_table[val][1]);

			for (size_t i = 0; i < num_bytes; i += sizeof(__m128i))
			{
				// in_val = in[i]
				__m128i in_vals = _mm_load_si128((const __m128i*)(in + i));
				// idx1 = in_val & 0xF
				__m128i idx1 = _mm_and_si128(in_vals, mask);
				// idx2 = (in_val >> 4) & 0xF
				__m128i shifted = _mm_srli_epi64(in_vals, 4);
				__m128i idx2 = _mm_and_si128(shifted, mask);
				// lo_val = lo[idx1]
				__m128i lo_vals = _mm_shuffle_epi8(lo, idx1);
				// hi_val = hi[idx2]
				__m128i hi_vals = _mm_shuffle_epi8(hi, idx2);
				// result = lo_val ^ hi_val
				__m128i result = _mm_xor_si128(lo_vals, hi_vals);
				// out[i] ^= result
				result = _mm_xor_si128(result, _mm_load_si128((__m128i*)(out + i)));
				_mm_store_si128((__m128i*)(out + i), result);
			}
		}
		void mul_row(
			uint8_t val, 
			const uint8_t* in, 
			uint8_t* out,
			size_t num_bytes)
		{
			const __m128i mask = _mm_set1_epi8(0x0F);

			__m128i lo = _mm_load_si128((const __m128i*)lohi_table[val][0]);
			__m128i hi = _mm_load_si128((const __m128i*)lohi_table[val][1]);

			for (size_t i = 0; i < num_bytes; i += 16)
			{
				// in_val = in[i]
				__m128i in_vals = _mm_load_si128((const __m128i*)(in + i));
				// idx1 = in_val & 0xF
				__m128i idx1 = _mm_and_si128(in_vals, mask);
				// idx2 = (in_val >> 4) & 0xF
				__m128i shifted = _mm_srli_epi64(in_vals, 4);
				__m128i idx2 = _mm_and_si128(shifted, mask);
				// lo_val = lo[idx1]
				__m128i lo_vals = _mm_shuffle_epi8(lo, idx1);
				// hi_val = hi[idx2]
				__m128i hi_vals = _mm_shuffle_epi8(hi, idx2);
				// result = lo_val ^ hi_val
				__m128i result = _mm_xor_si128(lo_vals, hi_vals);
				// out[i] = result
				_mm_store_si128((__m128i*)(out + i), result);
			}
		}
	}

	namespace
	{
		constexpr size_t round_mask = ~(15);

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
[0x00000004]	0xa8 '¨'	char
[0x00000005]	0x82 '‚'	char
[0x00000006]	0xfc 'ü'	char
[0x00000007]	0xd6 'Ö'	char
[0x00000008]	0x4d 'M'	char
[0x00000009]	0x67 'g'	char
[0x0000000a]	0x19 '\x19'	char
[0x0000000b]	0x33 '3'	char
[0x0000000c]	0xe5 'å'	char
[0x0000000d]	0xcf 'Ï'	char
[0x0000000e]	0xb1 '±'	char
[0x0000000f]	0x9b '›'	char
-		lo	{m128i_i8=0x000000b54d6ff8d0 "" m128i_i16=0x000000b54d6ff8d0 {0x2100, 0x6342, 0xa584, 0xe7c6, 0x3415, ...} ...}	__m128i
[0x00000000]	0x00 '\0'	char
[0x00000001]	0x21 '!'	char
[0x00000002]	0x42 'B'	char
[0x00000003]	0x63 'c'	char
[0x00000004]	0x84 '„'	char
[0x00000005]	0xa5 '¥'	char
[0x00000006]	0xc6 'Æ'	char
[0x00000007]	0xe7 'ç'	char
[0x00000008]	0x15 '\x15'	char
[0x00000009]	0x34 '4'	char
[0x0000000a]	0x57 'W'	char
[0x0000000b]	0x76 'v'	char
[0x0000000c]	0x91 '‘'	char
[0x0000000d]	0xb0 '°'	char
[0x0000000e]	0xd3 'Ó'	char
[0x0000000f]	0xf2 'ò'	char
-		hi_vals	{m128i_i8=0x000000b54d6ff9c0 "±±‚›\x19*3~\x19‚*‚‚~ÏM... m128i_i16=0x000000b54d6ff9c0 {0xb1b1, 0x9b82, ...} ...}	__m128i
[0x00000000]	0xb1 '±'	char
[0x00000001]	0xb1 '±'	char
[0x00000002]	0x82 '‚'	char
[0x00000003]	0x9b '›'	char
[0x00000004]	0x19 '\x19'	char
[0x00000005]	0x2a '*'	char
[0x00000006]	0x33 '3'	char
[0x00000007]	0x7e '~'	char
[0x00000008]	0x19 '\x19'	char
[0x00000009]	0x82 '‚'	char
[0x0000000a]	0x2a '*'	char
[0x0000000b]	0x82 '‚'	char
[0x0000000c]	0x82 '‚'	char
[0x0000000d]	0x7e '~'	char
[0x0000000e]	0xcf 'Ï'	char
[0x0000000f]	0x4d 'M'	char
-		lo_vals	{m128i_i8=0x000000b54d6ff9a0 "ç„4W!!cçWB\x154‘v‘°... m128i_i16=0x000000b54d6ff9a0 {0x84e7, 0x5734, 0x2121, ...} ...}	__m128i
[0x00000000]	0xe7 'ç'	char
[0x00000001]	0x84 '„'	char
[0x00000002]	0x34 '4'	char
[0x00000003]	0x57 'W'	char
[0x00000004]	0x21 '!'	char
[0x00000005]	0x21 '!'	char
[0x00000006]	0x63 'c'	char
[0x00000007]	0xe7 'ç'	char
[0x00000008]	0x57 'W'	char
[0x00000009]	0x42 'B'	char
[0x0000000a]	0x15 '\x15'	char
[0x0000000b]	0x34 '4'	char
[0x0000000c]	0x91 '‘'	char
[0x0000000d]	0x76 'v'	char
[0x0000000e]	0x91 '‘'	char
[0x0000000f]	0xb0 '°'	char

*/
