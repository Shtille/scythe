#include <scythe/endianness.h>

namespace scythe {

	bool IsLittleEndian()
	{
		int n = 1;
		return *(char*)&n == 1;
	}
	bool IsBigEndian()
	{
		return !IsLittleEndian();
	}

} // namespace scythe