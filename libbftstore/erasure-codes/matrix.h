#pragma once

#include "symbol.h"
#include "export_defs.h"

#include <matrix.h>

#include <cstring>
#include <cstdlib>
#include <cassert>

namespace erasure
{
	using gfarith::matrix;

	bool inverse(matrix& mat);

	matrix vandermonde(size_t n, size_t k);
	matrix build_matrix(size_t n, size_t k);
}
