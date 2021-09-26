#include "matrix.h"

#include <cstdlib>
#include <cstring>

namespace gfarith
{
	symbol* matrix::alloc_values() const
	{
		return (symbol*)std::malloc(datasize());
	}

	matrix::matrix() :
		rows(0),
		cols(0),
		values(nullptr)
	{

	}
	matrix::matrix(size_t rows, size_t cols) :
		rows(rows),
		cols(cols)
	{
		values = alloc_values();
	}
	matrix::matrix(size_t rows, size_t cols, symbol diag) :
		rows(rows),
		cols(cols)
	{
		values = alloc_values();

		if (values != nullptr)
		{
			std::memset(values, 0, datasize());

			size_t mindim = rows < cols ? rows : cols;
			for (size_t i = 0; i < mindim; ++i)
			{
				(*this)(i, i) = diag;
			}
		}
	}

	matrix::matrix(const matrix& m) :
		rows(m.rows),
		cols(m.cols)
	{
		values = alloc_values();

		std::memcpy(values, m.values, datasize());
	}
	matrix::matrix(matrix&& m) :
		rows(m.rows),
		cols(m.cols)
	{
		values = m.values;

		m.values = nullptr;
		m.rows = m.cols = 0;
	}

	matrix::~matrix()
	{
		free(values);
	}

	matrix& matrix::operator=(const matrix& m)
	{
		if (m.values == values)
			return *this;

		if (!values)
		{
			values = alloc_values();
		}
		else
		{
			values = (symbol*)std::realloc(values, datasize());
		}

		if (!values)
		{
			// Memory allocation failed 
			rows = 0;
			cols = 0;
		}
		else
		{
			std::memcpy(values, m.values, datasize());
		}

		return *this;
	}
	matrix& matrix::operator=(matrix&& m)
	{
		if (values == m.values)
			return *this;

		if (values)
			free(values);

		rows = m.rows;
		cols = m.cols;
		values = m.values;

		m.rows = m.cols = 0;
		m.values = nullptr;

		return *this;
	}

	matrix matrix::submatrix(size_t r1, size_t r2, size_t c1, size_t c2) const
	{
		assert(r1 <= r2 && r2 <= rows);
		assert(c1 <= c2 && c2 <= cols);
		
		if (is_null())
			return matrix();

		matrix m{ r2 - r1, c2 - c1 };

		for (size_t r = 0; r < m.size1(); ++r)
		{
			std::memcpy(m[r].data(), (*this)[r1 + r].data() + c1, m.size2());
		}

		return m;
	}

	matrix operator*(const matrix& a, const matrix& b)
	{
		if (a.is_null() || b.is_null())
			return matrix();

		assert(a.size2() == b.size1());

		matrix result(a.size1(), b.size2());

		for (size_t i = 0; i < a.size1(); ++i)
		{
			for (size_t j = 0; j < b.size2(); ++j)
			{
				symbol& sum = result(i, j) = 0;

				for (size_t k = 0; k < a.size2(); ++k)
				{
					sum += a(i, k) * b(k, j);
				}
			}
		}

		return result;
	}

	matrix_row& matrix_row::operator=(const matrix_row& m)
	{
		assert(this->size1() == m.size1());

		std::memcpy(this->data(), m.data(), m.size() * sizeof(symbol));

		return *this;
	}

	bool operator==(const matrix& a, const matrix& b)
	{
		if (a.size1() != b.size1() || a.size2() != b.size2())
			return false;
		if (a.is_null() && b.is_null())
			return true;
		if (a.is_null() || b.is_null())
			return false;

		return std::memcmp(a.data(), b.data(), a.size() * sizeof(symbol)) == 0;
	}
	bool operator !=(const matrix& a, const matrix& b)
	{
		return !(a == b);
	}
}
