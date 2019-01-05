#include "bounding_box.h"

namespace scythe {
	
inline BoundingBox& BoundingBox::operator*=(const Matrix4& matrix)
{
	Transform(matrix);
	return *this;
}

inline const BoundingBox operator*(const Matrix4& matrix, const BoundingBox& box)
{
	BoundingBox b(box);
	b.Transform(matrix);
	return b;
}

} // namespace scythe