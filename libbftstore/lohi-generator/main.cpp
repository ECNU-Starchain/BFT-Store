#include "symbol.h"

#include <iostream>
#include <fstream>

using namespace gfarith;

int main(int argc, char** argv)
{
	if (argc < 2)
		return -1;

	std::ofstream ss{ argv[1] };

	ss << "#include <cstdint>\n\n"
		<< "namespace gfarith\n{\n";

	ss << "\talignas(64) const uint8_t lohi_table[256][2][16] = {\n" << std::hex;

	bool b1 = false, b2 = false;
	for (size_t i = 0; i < 256; ++i)
	{
		if (b1)
			ss << ",\n";
		else
			b1 = true;

		ss << "\t\t{ {";

		b2 = false;
		for (size_t j = 0; j < 16; ++j)
		{
			if (b2)
				ss << ',';
			else
				b2 = true;

			ss << "0x" << (size_t)mul((uint8_t)i, (uint8_t)j);
		}

		ss << "}, {";

		b2 = false;
		for (size_t j = 0; j < 16; ++j)
		{
			if (b2)
				ss << ',';
			else
				b2 = true;

			ss << "0x" << (size_t)mul((uint8_t)i, (uint8_t)j << 4);
		}

		ss << "} }";
	}

	ss << "\n\t};\n}\n";
	
	return 0;
}
