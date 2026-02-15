#include "ray.h"

namespace scythe {
	
inline Ray& Ray::operator*=(const Matrix4& matrix)
{
	Transform(matrix);
	return *this;
}

inline const Ray operator*(const Matrix4& matrix, const Ray& ray)
{
	Ray r(ray);
	r.Transform(matrix);
	return r;
}

} // namespace scythe