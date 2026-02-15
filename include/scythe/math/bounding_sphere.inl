#include "bounding_sphere.h"

namespace scythe {
	
inline BoundingSphere& BoundingSphere::operator*=(const Matrix4& matrix)
{
	Transform(matrix);
	return *this;
}

inline const BoundingSphere operator*(const Matrix4& matrix, const BoundingSphere& sphere)
{
	BoundingSphere s(sphere);
	s.Transform(matrix);
	return s;
}

} // namespace scythe