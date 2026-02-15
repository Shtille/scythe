#ifndef __SCYTHE_PLANE_H__
#define __SCYTHE_PLANE_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include "vector3.h"

namespace scythe {

class Matrix4;
class BoundingSphere;
class BoundingBox;
class Frustum;
class Ray;

/**
 * Defines a plane which is a flat surface with 2 sides.
 *
 * The plane is represented as a plane using a 3D vector normal and a
 * distance value (stored as a negative value).
 */
class Plane
{
public:

	/**
	 * Represents when a 3D entity intersects a plane.
	 */
	static const int kIntersectionExists = 0;

	/**
	 * Represents when a 3D entity is in front of (in the positive half-space of) a plane.
	 */
	static const int kIntersectionFront = 1;

	/**
	 * Represents when a 3D entity is behind (in the negative half-space of) a plane.
	 */
	static const int kIntersectionBack = -1;

	/**
	 * Constructs a new plane with normal (0, 1, 0) and distance 0.
	 */
	Plane();

	/**
	 * Constructs a new plane from the specified values.
	 *
	 * @param normal The normal vector of this plane.
	 * @param distance The distance from this plane along its (unit) normal to the origin.
	 */
	Plane(const Vector3& normal, float distance);

	/**
	 * Constructs a new plane from the specified values.
	 * 
	 * @param normalX The x coordinate of the normal.
	 * @param normalY The y coordinate of the normal.
	 * @param normalZ The z coordinate of the normal.
	 * @param distance The distance from this plane along its (unit) normal to the origin.
	 */
	Plane(float normalX, float normalY, float normalZ, float distance);

	/**
	 * Constructs a new plane from the given plane.
	 *
	 * @param copy The plane to copy.
	 */
	Plane(const Plane& copy);

	/**
	 * Destructor.
	 */
	~Plane();

	/**
	 * Gets the plane's normal in the given vector.
	 *
	 * @return normal The plane's normal.
	 */
	const Vector3& GetNormal() const;

	/**
	 * Sets the plane's normal to the given vector.
	 *
	 * @param normal The new normal vector.
	 */
	void SetNormal(const Vector3& normal);

	/**
	 * Sets the plane's normal.
	 * 
	 * @param x The x coordinate of the normal.
	 * @param y The y coordinate of the normal.
	 * @param z The z coordinate of the normal.
	 */
	void SetNormal(float x, float y, float z);

	/**
	 * Gets the plane's distance to the origin along its normal.
	 *
	 * @return The plane's distance to the origin along its normal.
	 */
	float GetDistance() const;

	/**
	 * Sets the plane's distance to the origin along its normal.
	 *
	 * @param distance The new distance.
	 */
	void SetDistance(float distance);

	/**
	 * Calculates the distance from this plane to the specified point.
	 *
	 * @param point The point to calculate distance to.
	 */
	float Distance(const Vector3& point) const;

	/**
	 * Calculates the point of intersection of the given three planes and stores it in the given point.
	 *
	 * @param p1 The first plane.
	 * @param p2 The second plane.
	 * @param p3 The third plane.
	 * @param point The point to store the point of intersection in
	 *  (this is left unmodified if the planes do not all intersect or if
	 *  they are all parallel along one vector and intersect along a line).
	 */
	static void Intersection(const Plane& p1, const Plane& p2, const Plane& p3, Vector3* point);

	/**
	 * Tests whether this plane intersects the specified bounding sphere.
	 *
	 * @param sphere The bounding sphere to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if the specified bounding object is in the negative half-space of
	 *  this plane, Plane::kIntersectionFront if it is in the positive half-space of this plane,
	 *  and Plane::kIntersectionExists if it intersects this plane.
	 */
	int Intersects(const BoundingSphere& sphere) const;

	/**
	 * Tests whether this plane intersects the specified bounding box.
	 *
	 * @param box The bounding box to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if the specified bounding object is in the negative half-space of
	 *  this plane, Plane::kIntersectionFront if it is in the positive half-space of this plane,
	 *  and Plane::kIntersectionExists if it intersects this plane.
	 */
	int Intersects(const BoundingBox& box) const;

	/**
	 * Tests whether this plane intersects the specified frustum.
	 *
	 * @param frustum The frustum to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if the specified frustum is in the negative half-space of
	 *  this plane, Plane::kIntersectionFront if it is in the positive half-space of this plane,
	 *  and Plane::kIntersectionExists if it intersects this plane.
	 */
	int Intersects(const Frustum& frustum) const;

	/**
	 * Tests whether this plane intersects the specified plane.
	 *
	 * @param plane The plane to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if the specified plane is in the negative half-space of
	 *  this plane, Plane::kIntersectionFront if it is in the positive half-space of this plane,
	 *  and Plane::kIntersectionExists if it intersects this plane.
	 */
	int Intersects(const Plane& plane) const;

	/**
	 * Tests whether this plane intersects the specified ray.
	 *
	 * @param ray The ray to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if the specified ray is in the negative half-space of
	 *  this plane, Plane::kIntersectionFront if it is in the positive half-space of this plane,
	 *  and Plane::kIntersectionExists if it intersects this plane.
	 */
	int Intersects(const Ray& ray) const;

	/**
	 * Determines whether the given plane is parallel to this plane.
	 * 
	 * @param plane The plane to test.
	 * 
	 * @return true if the given plane is parallel to this plane; false otherwise.
	 */
	bool IsParallel(const Plane& plane) const;

	/**
	 * Sets this plane to the specified values.
	 *
	 * @param normal The normal vector of this plane.
	 * @param distance The distance to this plane along its normal to the origin.
	 */
	void Set(const Vector3& normal, float distance);

	/**
	 * Sets this plane to the given plane.
	 *
	 * @param plane The plane to copy.
	 */
	void Set(const Plane& plane);

	/**
	 * Transforms this plane by the given transformation matrix.
	 *
	 * @param matrix The transformation matrix to transform by.
	 */
	void Transform(const Matrix4& matrix);

	/**
	 * Transforms this plane by the given matrix.
	 * 
	 * @param matrix The matrix to transform by.
	 * @return This plane, after the transformation occurs.
	 */
	inline Plane& operator*=(const Matrix4& matrix);

private:

	/**
	 * Normalizes this plane.
	 */
	void Normalize();

	Vector3 normal_;    //<! The normal vector of the Plane.
	float distance_;    //<! The distance of the Plane along its normal from the origin.
};

/**
 * Transforms the given plane by the given matrix.
 * 
 * @param matrix The matrix to transform by.
 * @param plane The plane to transform.
 * @return The resulting transformed plane.
 */
inline const Plane operator*(const Matrix4& matrix, const Plane& plane);

} // namespace scythe

#include "plane.inl"

#endif