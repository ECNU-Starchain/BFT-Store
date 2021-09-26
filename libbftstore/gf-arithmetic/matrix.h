#ifndef GFARITH_MATRIX_H
#define GFARITH_MATRIX_H

#include "symbol.h"

#include <cstddef>
#include <cassert>

namespace gfarith
{
	using std::size_t;

	class matrix_row
	{
	private:
		size_t length;
		symbol* values;

	public:
		matrix_row(size_t length, symbol* values) :
			length(length),
			values(values)
		{

		}

		symbol& operator[](size_t i)
		{
			assert(i < length);

			return values[i];
		}
		const symbol& operator[](size_t i) const
		{
			assert(i < length);

			return values[i];
		}

		size_t size1() const
		{
			return 1;
		}
		size_t size2() const
		{
			return length;
		}
		size_t size() const
		{
			return size1() * size2();
		}

		symbol* data()
		{
			return values;
		}
		const symbol* data() const
		{
			return values;
		}

		matrix_row& operator=(const matrix_row& r);

		symbol* begin()
		{
			return values;
		}
		symbol* end()
		{
			return values + length;
		}
		const symbol* begin() const
		{
			return values;
		}
		const symbol* end() const
		{
			return values + length;
		}
	};

	class matrix
	{
	private:
		size_t rows;
		size_t cols;
		symbol* values;

		size_t datasize() const
		{
			return rows * cols * sizeof(symbol);
		}
		symbol* alloc_values() const;

	public:
		matrix();
		matrix(size_t rows, size_t cols);
		matrix(size_t rows, size_t cols, symbol diag);

		matrix(const matrix& m);
		matrix(matrix&&);

		~matrix();

		matrix& operator=(const matrix&);
		matrix& operator=(matrix&&);

		symbol& operator()(size_t r, size_t c)
		{
			assert(!is_null());
			assert(r < rows);
			assert(c < cols);

			return values[r * cols + c];
		}
		const symbol& operator()(size_t r, size_t c) const
		{
			assert(!is_null());
			assert(r < rows);
			assert(c < cols);

			return values[r * cols + c];
		}

		matrix_row operator[](size_t r)
		{
			assert(r < rows);
			assert(!is_null());

			return matrix_row(cols, values + r * cols);
		}
		const matrix_row operator[](size_t r) const
		{
			assert(r < rows);
			assert(!is_null());

			return matrix_row(cols, values + r * cols);
		}

		matrix submatrix(size_t r1, size_t r2, size_t c1, size_t c2) const;

		matrix inverse() const;

		size_t size1() const
		{
			return rows;
		}
		size_t size2() const
		{
			return cols;
		}
		size_t size() const
		{
			return size1() * size2();
		}

		symbol* data()
		{
			return values;
		}
		const symbol* data() const
		{
			return values;
		}

		bool is_null() const
		{
			return values == 0;
		}
	};

	matrix operator*(const matrix& a, const matrix& b);

	bool operator==(const matrix& a, const matrix& b);
	bool operator!=(const matrix& a, const matrix& b);
}

#endif
