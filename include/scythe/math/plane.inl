#include "plane.h"

namespace scythe {
	
inline Plane& Plane::operator*=(const Matrix4& matrix)
{
	Transform(matrix);
	return *this;
}

inline const Plane operator*(const Matrix4& matrix, const Plane& plane)
{
	Plane p(plane);
	p.Transform(matrix);
	return p;
}

} // namespace scythe