#include "matrix.h"
#include "encode.h"

#include <cstring>
#include <algorithm>

namespace gfarith
{	
	matrix matrix::inverse() const
	{
		if (this->is_null())
			return matrix();

		assert(size1() == size2());

		matrix m = matrix(size1(), size2() * 2);

		std::memset(m.data(), 0, m.datasize());

		// Initialize the extended matrix
		for (size_t r = 0; r < m.size1(); ++r)
		{
			// Copy our matrix value
			std::copy((*this)[r].begin(), (*this)[r].end(), m[r].begin());
			// Inverse is initialized to the identity matrix
			m(r, r + size2()) = 1;
		}

		for (size_t r1 = 0; r1 < m.size1(); ++r1)
		{
			symbol div = m(r1, r1);
			// 保证对角线元素不为0
			if (div.value == 0)
			{
				matrix tmp(1, m.size2());

				// Swap the row with one that doesn't have a 0
				// along the diagonal
				for (size_t r2 = r1 + 1; r2 < m.size1(); ++r2)
				{
					if (m(r2, r1).value != 0)
					{
						// Swap the rows using a matrix temporary
						tmp[0] = m[r1];
						m[r1] = m[r2];
						m[r2] = tmp[0];

						break;
					}
				}

				div = m(r1, r1);

				if (div.value == 0)
					return matrix();
			}
			//保证对角线元素为1
			if (div.value != 1)
			{
				for (size_t c = r1; c < m.size2(); ++c)
				{
					m(r1, c) /= div;
				}
			}

			for (size_t r2 = 0; r2 < m.size1(); ++r2)
			{
				if (r2 == r1)
					continue;

				symbol mult = m(r2, r1);

				for (size_t c = 0; c < m.size2(); ++c)
				{
					m(r2, c) -= mult * m(r1, c);
				}
			}
		}

		return m.submatrix(0, size1(), size1(), size1() * 2);
	}
}
