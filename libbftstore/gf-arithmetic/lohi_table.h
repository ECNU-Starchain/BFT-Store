#ifndef GFARITH_LOHI_TABLE_H
#define GFARITH_LOHI_TABLE_H

#include <cstdint>

namespace gfarith
{
	alignas(64) const extern uint8_t lohi_table[256][2][16];
}

#endif
