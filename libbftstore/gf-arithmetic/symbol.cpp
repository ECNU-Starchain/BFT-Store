#include "symbol.h"

#include <cassert>

namespace gfarith
{
	uint8_t add(uint8_t a, uint8_t b)
	{
		return a ^ b;
	}
	uint8_t sub(uint8_t a, uint8_t b)
	{
		return a ^ b;
	}

	extern uint8_t log_table[256];
	extern uint8_t exp_table[256];

	uint8_t mul(uint8_t a, uint8_t b)
	{
		if (a == 0 || b == 0)
			return 0;

		uint8_t log_a = log_table[a];
		uint8_t log_b = log_table[b];

		auto result = log_a + log_b;

		return exp_table[result % 255];
	}
	uint8_t div(uint8_t a, uint8_t b)
	{
		if (a == 0)
			return 0;
		assert(b != 0);

		uint8_t log_a = log_table[a];
		uint8_t log_b = log_table[b];

		auto result = log_a - log_b;

		result += result < 0 ? 255 : 0;

		return exp_table[result];
	}

	uint8_t exp(uint8_t a, uint8_t n)
	{
		if (n == 0)
			return 1;
		if (a == 0)
			return 0;

		uint32_t log_a = log_table[a];
		uint32_t result = log_a * n;
		return exp_table[result % 255];
	}
}