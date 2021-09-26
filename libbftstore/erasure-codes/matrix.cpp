
// Disable debug checks in ublas
// our type doesn't suffer from 
// precision issues anyway
#define BOOST_UBLAS_NDEBUG
// Disable exceptions in boost
// ublas methods
#define BOOST_NO_EXCEPTIONS

#ifdef _MSC_VER
#	pragma warning(disable:4267 4996)
#endif

#include "matrix.h"

namespace erasure
{
	bool inverse(matrix& m)
	{
		matrix inv = m.inverse();

		if (inv.is_null())
			return false;

		m = inv;

		return true;
	}

	matrix vandermonde(size_t n, size_t k)
	{
		matrix m{ n, k };
		
		for (uint8_t r = 0; r < n; ++r)
		{
			for (uint8_t c = 0; c < k; ++c)
			{
				m(r, c) = gfarith::exp(r, c);
			}
		}

		return m;
	}
	matrix build_matrix(size_t n, size_t k)
	{
		auto vm = vandermonde(n, k);
		matrix inv = vm.submatrix(0, k, 0, k);

		auto r = inverse(inv);
		assert(r); // Matrix should never be singular

		return vm * inv;
	}
}
