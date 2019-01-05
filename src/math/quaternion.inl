#include "quaternion.h"

namespace scythe {
	
	inline const Quaternion Quaternion::operator*(const Quaternion& q) const
	{
		Quaternion result(*this);
		result.Multiply(q);
		return result;
	}

	inline Quaternion& Quaternion::operator*=(const Quaternion& q)
	{
		Multiply(q);
		return *this;
	}

} // namespace scythe