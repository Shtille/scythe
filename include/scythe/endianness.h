#ifndef __SCYTHE_ENDIANNESS_H__
#define __SCYTHE_ENDIANNESS_H__

#include "types.h"

namespace scythe {

	bool IsLittleEndian();
	bool IsBigEndian();

	inline U16 SwapU16(U16 value)
	{
		return ((value & 0x00FF) << 8)
			 | ((value & 0xFF00) >> 8);
	}
	inline U32 SwapU32(U32 value)
	{
		return ((value & 0x000000FF) << 24)
			 | ((value & 0x0000FF00) << 8)
			 | ((value & 0x00FF0000) >> 8)
			 | ((value & 0xFF000000) >> 24);
	}

	union U32F32
	{
		U32 as_U32;
		F32 as_F32;
	};
	inline F32 SwapF32(F32 value)
	{
		U32F32 u;
		u.as_F32 = value;
		u.as_U32 = SwapU32(u.as_U32);
		return u.as_F32;
	}

} // namespace scythe

#endif