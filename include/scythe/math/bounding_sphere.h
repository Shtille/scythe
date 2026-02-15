#ifndef __SCYTHE_BOUNDING_SPHERE_H__
#define __SCYTHE_BOUNDING_SPHERE_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include "vector3.h"

namespace scythe {

class BoundingBox;
class Frustum;
class Plane;
class Ray;
class Matrix4;

/**
 * Defines a 3-dimensional bounding sphere.
 */
class BoundingSphere
{
public:

	/**
	 * The center point.
	 */
	Vector3 center;

	/**
	 * The sphere radius.
	 */
	float radius;

	/**
	 *  Constructs a new bounding sphere initialized to all zeros.
	 */
	BoundingSphere();

	/**
	 * Constructs a new bounding sphere initialized to the specified values.
	 *
	 * @param center The center of the sphere.
	 * @param radius The radius of the sphere.
	 */
	BoundingSphere(const Vector3& center, float radius);

	/**
	 * Constructs a bounding sphere from the given bounding sphere.
	 *
	 * @param copy The bounding sphere to copy.
	 */
	BoundingSphere(const BoundingSphere& copy);

	/**
	 * Destructor.
	 */
	~BoundingSphere();

	/**
	 * Returns an empty bounding sphere.
	 */
	static const BoundingSphere& Empty();

	/**
	 * Tests whether this bounding sphere intersects the specified bounding sphere.
	 *
	 * @param sphere The bounding sphere to test intersection with.
	 * 
	 * @return true if the specified bounding sphere intersects this bounding sphere; false otherwise.
	 */
	bool Intersects(const BoundingSphere& sphere) const;

	/**
	 * Tests whether this bounding sphere intersects the specified bounding box.
	 *
	 * @param box The bounding box to test intersection with.
	 * 
	 * @return true if the specified bounding box intersects this bounding sphere; false otherwise.
	 */
	bool Intersects(const BoundingBox& box) const;

	/**
	 * Tests whether this bounding sphere intersects the specified frustum.
	 *
	 * @param frustum The frustum to test intersection with.
	 * 
	 * @return true if this bounding sphere intersects the specified frustum; false otherwise.
	 */
	bool Intersects(const Frustum& frustum) const;

	/**
	 * Tests whether this bounding sphere intersects the specified plane.
	 *
	 * @param plane The plane to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if this bounding sphere is in the negative half-space of
	 *  the plane, Plane::kIntersectionFront if it is in the positive half-space of the plane,
	 *  and Plane::kIntersectionExists if it intersects the plane.
	 */
	int Intersects(const Plane& plane) const;

	/**
	 * Tests whether this bounding sphere intersects the specified ray.
	 *
	 * @param[in] ray The ray to test intersection with.
	 * @param[opt] distance The distance from the origin of the ray to this bounding sphere.
	 * 
	 * @return True if this ray intersects the bounding object and false otherwise.
	 */
	bool Intersects(const Ray& ray, float* distance) const;

	/**
	 * Determines if this bounding sphere is empty.
	 *
	 * @return true if this bounding sphere is empty; false otherwise.
	 */
	bool IsEmpty() const;

	/**
	 * Sets this bounding sphere to the smallest bounding sphere
	 * that contains both this bounding sphere and the specified bounding sphere.
	 *
	 * @param sphere The bounding sphere to merge with.
	 */
	void Merge(const BoundingSphere& sphere);

	/**
	 * Sets this bounding sphere to the smallest bounding sphere
	 * that contains both this bounding sphere and the specified bounding box.
	 *
	 * @param box The bounding box to merge with.
	 */
	void Merge(const BoundingBox& box);

	/**
	 * Sets this bounding sphere to the specified values.
	 *
	 * @param center The center of the sphere.
	 * @param radius The radius of the sphere.
	 */
	void Set(const Vector3& center, float radius);

	/**
	 * Sets this bounding sphere to the specified bounding sphere.
	 *
	 * @param sphere The bounding sphere to set to.
	 */
	void Set(const BoundingSphere& sphere);

	/**
	 * Sets this bounding sphere to tightly contain the specified bounding box.
	 *
	 * @param box The box to contain.
	 */
	void Set(const BoundingBox& box);

	/**
	 * Transforms the bounding sphere by the given transformation matrix.
	 *
	 * @param matrix The transformation matrix to transform by.
	 */
	void Transform(const Matrix4& matrix);

	/**
	 * Transforms this bounding sphere by the given matrix.
	 * 
	 * @param matrix The matrix to transform by.
	 * @return This bounding sphere, after the transformation occurs.
	 */
	inline BoundingSphere& operator*=(const Matrix4& matrix);

private:

	float Distance(const BoundingSphere& sphere, const Vector3&);

	bool Contains(const BoundingSphere& sphere, const Vector3* points, unsigned int count);
};

/**
 * Transforms the given bounding sphere by the given matrix.
 * 
 * @param matrix The matrix to transform by.
 * @param sphere The bounding sphere to transform.
 * @return The resulting transformed bounding sphere.
 */
inline const BoundingSphere operator*(const Matrix4& matrix, const BoundingSphere& sphere);

} // namespace scythe

#include "bounding_sphere.inl"

#endif