#ifndef __SCYTHE_RAY_H__
#define __SCYTHE_RAY_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include "vector3.h"

namespace scythe {

class Matrix4;
class Frustum;
class Plane;
class BoundingSphere;
class BoundingBox;

/**
 * Defines a 3-dimensional ray.
 *
 * Rays direction vector are always normalized.
 */
class Ray
{
public:

	/**
	 * Constructs a new ray initialized to origin(0,0,0) and direction(0,0,1).
	 */
	Ray();

	/**
	 * Constructs a new ray initialized to the specified values.
	 *
	 * @param origin The ray's origin.
	 * @param direction The ray's direction.
	 */
	Ray(const Vector3& origin, const Vector3& direction);

	/**
	 * Constructs a new ray initialized to the specified values.
	 * 
	 * @param originX The x coordinate of the origin.
	 * @param originY The y coordinate of the origin.
	 * @param originZ The z coordinate of the origin.
	 * @param dirX The x coordinate of the direction.
	 * @param dirY The y coordinate of the direction.
	 * @param dirZ The z coordinate of the direction.
	 */
	Ray(float originX, float originY, float originZ, float dirX, float dirY, float dirZ);

	/**
	 * Constructs a new ray from the given ray.
	 *
	 * @param copy The ray to copy.
	 */
	Ray(const Ray& copy);

	/**
	 * Destructor.
	 */
	~Ray();

	/**
	 * Gets the ray's origin.
	 *
	 * @return The ray's origin.
	 */
	const Vector3& GetOrigin() const;

	/**
	 * Sets the ray's origin to the given point.
	 *
	 * @param origin The new origin.
	 */
	void SetOrigin(const Vector3& origin);

	/**
	 * Sets the ray's origin.
	 * 
	 * @param x The x coordinate of the origin.
	 * @param y The y coordinate of the origin.
	 * @param z The z coordinate of the origin.
	 */
	void SetOrigin(float x, float y, float z);

	/**
	 * Gets the ray's direction.
	 *
	 * @return The ray's direction.
	 */
	const Vector3& GetDirection() const;

	/**
	 * Sets the ray's direction to the given vector.
	 *
	 * @param direction The new direction vector.
	 */
	void SetDirection(const Vector3& direction);

	/**
	 * Sets the ray's direction.
	 * 
	 * @param x The x coordinate of the direction.
	 * @param y The y coordinate of the direction.
	 * @param z The z coordinate of the direction.
	 */
	void SetDirection(float x, float y, float z);

	/**
	 * Tests whether this ray intersects the specified bounding sphere.
	 *
	 * @param[in] sphere The bounding sphere to test intersection with.
	 * @param[opt,out] distance The distance from the origin of this ray to the bounding object.
	 * 
	 * @return True if this ray intersects the bounding object and false otherwise.
	 */
	bool Intersects(const BoundingSphere& sphere, float* distance) const;

	/**
	 * Tests whether this ray intersects the specified bounding box.
	 *
	 * @param[in] box The bounding box to test intersection with.
	 * @param[opt,out] distance The distance from the origin of this ray to the bounding object.
	 * 
	 * @return True if this ray intersects the bounding object and false otherwise.
	 */
	bool Intersects(const BoundingBox& box, float* distance) const;

	/**
	 * Tests whether this ray intersects the specified frustum.
	 *
	 * @param[in] frustum The frustum to test intersection with.
	 * @param[opt,out] distance The distance from the origin of this ray to the bounding object.
	 * 
	 * @return True if this ray intersects the bounding object and false otherwise.
	 */
	bool Intersects(const Frustum& frustum, float* distance) const;

	/**
	 * Tests whether this ray intersects the specified plane and returns the distance
	 * from the origin of the ray to the plane.
	 *
	 * @param[in] plane The plane to test intersection with.
	 * @param[opt,out] distance The distance from the origin of this ray to the bounding object.
	 * 
	 * @return True if this ray intersects the bounding object and false otherwise.
	 */
	bool Intersects(const Plane& plane, float* distance) const;

	/**
	 * Sets this ray to the specified values.
	 *
	 * @param origin The ray's origin.
	 * @param direction The ray's direction.
	 */
	void Set(const Vector3& origin, const Vector3& direction);

	/**
	 * Sets this ray to the given ray.
	 *
	 * @param ray The ray to copy.
	 */
	void Set(const Ray& ray);

	/**
	 * Transforms this ray by the given transformation matrix.
	 *
	 * @param matrix The transformation matrix to transform by.
	 */
	void Transform(const Matrix4& matrix);

	/**
	 * Transforms this ray by the given matrix.
	 * 
	 * @param matrix The matrix to transform by.
	 * @return This ray, after the transformation occurs.
	 */
	inline Ray& operator*=(const Matrix4& matrix);

private:

	/**
	 * Normalizes the ray.
	 */
	void Normalize();

	Vector3 origin_;        //!< The ray origin position.
	Vector3 direction_;     //!< The ray direction vector.
};

/**
 * Transforms the given ray by the given matrix.
 * 
 * @param matrix The matrix to transform by.
 * @param ray The ray to transform.
 * @return The resulting transformed ray.
 */
inline const Ray operator*(const Matrix4& matrix, const Ray& ray);

} // namespace scythe

#include "ray.inl"

#endif