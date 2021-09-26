#ifndef GFARITH_SYMBOL_H
#define GFARITH_SYMBOL_H

#include <cstdint>

namespace gfarith
{
	struct symbol
	{
		uint8_t value;

		symbol() = default;
		symbol(uint8_t v) :
			value(v)
		{

		}
		
		explicit operator uint8_t() const
		{
			return value;
		}
	};

	uint8_t add(uint8_t a, uint8_t b);
	uint8_t sub(uint8_t a, uint8_t b);
	uint8_t mul(uint8_t a, uint8_t b);
	uint8_t div(uint8_t a, uint8_t b);
	uint8_t exp(uint8_t a, uint8_t b);

	inline symbol operator+(symbol a, symbol b)
	{
		return add(a.value, b.value);
	}
	inline symbol operator-(symbol a, symbol b)
	{
		return sub(a.value, b.value);
	}
	inline symbol operator*(symbol a, symbol b)
	{
		return mul(a.value, b.value);
	}
	inline symbol operator/(symbol a, symbol b)
	{
		return div(a.value, b.value);
	}

	inline symbol& operator+=(symbol& a, symbol b)
	{
		return a = a + b;
	}
	inline symbol& operator-=(symbol& a, symbol b)
	{
		return a = a - b;
	}
	inline symbol& operator*=(symbol& a, symbol b)
	{
		return a = a * b;
	}
	inline symbol& operator/=(symbol& a, symbol b)
	{
		return a = a / b;
	}
}

#endif
