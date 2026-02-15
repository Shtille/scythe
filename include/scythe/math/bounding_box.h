#ifndef __SCYTHE_BOUNDING_BOX_H__
#define __SCYTHE_BOUNDING_BOX_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include "vector3.h"

namespace scythe {

class BoundingSphere;
class Frustum;
class Plane;
class Ray;
class Matrix4;

/**
 * Defines a 3-dimensional axis-aligned bounding box.
 */
class BoundingBox
{
public:

	/**
	 * The minimum point.
	 */
	Vector3 min;

	/**
	 * The maximum point.
	 */
	Vector3 max;

	/**
	 * Constructs an empty bounding box at the origin.
	 */
	BoundingBox();

	/**
	 * Constructs a new bounding box from the specified values.
	 *
	 * @param min The minimum point of the bounding box.
	 * @param max The maximum point of the bounding box.
	 */
	BoundingBox(const Vector3& min, const Vector3& max);

	/**
	 * Constructs a new bounding box from the specified values.
	 * 
	 * @param minX The x coordinate of the minimum point of the bounding box.
	 * @param minY The y coordinate of the minimum point of the bounding box.
	 * @param minZ The z coordinate of the minimum point of the bounding box.
	 * @param maxX The x coordinate of the maximum point of the bounding box.
	 * @param maxY The y coordinate of the maximum point of the bounding box.
	 * @param maxZ The z coordinate of the maximum point of the bounding box.
	 */
	BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	/**
	 * Constructs a new bounding box from the given bounding box.
	 *
	 * @param copy The bounding box to copy.
	 */
	BoundingBox(const BoundingBox& copy);

	/**
	 * Destructor.
	 */
	~BoundingBox();

	/**
	 * Returns an empty bounding box.
	 */
	static const BoundingBox& Empty();

	/**
	 * Gets the center point of the bounding box.
	 *
	 * This method computes the center point of the box from its min and max.
	 *
	 * @return The center point of the bounding box.
	 */
	Vector3 GetCenter() const;

	/**
	 * Gets the center point of the bounding box.
	 *
	 * This method computes the center point of the box from its min and max
	 * points and stores the result in dst.
	 *
	 * @param dst The vector to store the result in.
	 */
	void GetCenter(Vector3* dst) const;

	/**
	 * Gets the corners of the bounding box in the specified array.
	 *
	 * The corners are returned as follows: 0 to 3 specify the near face starting at the upper left point
	 * when looking towards the origin from the positive z-axis in a counter-clockwise fashion; 4 to 7
	 * specify the far face starting at the upper left point when looking towards the origin from the negative
	 * z-axis in a counter-clockwise fashion.
	 *
	 * @param dst The array to store the corners in. Must be size 8.
	 */
	void GetCorners(Vector3* dst) const;

	/**
	 * Tests whether this bounding box intersects the specified bounding object.
	 *
	 * @param box The bounding box to test intersection with.
	 * 
	 * @return true if the specified bounding box intersects this bounding box; false otherwise.
	 */
	bool Intersects(const BoundingBox& box) const;

	/**
	 * Tests whether this bounding box intersects the specified bounding sphere.
	 *
	 * @param sphere The bounding sphere to test intersection with.
	 * 
	 * @return true if the specified bounding sphere intersects this bounding box; false otherwise.
	 */
	bool Intersects(const BoundingSphere& sphere) const;

	/**
	 * Tests whether this bounding box intersects the specified frustum.
	 *
	 * @param frustum The frustum to test intersection with.
	 * 
	 * @return true if this bounding sphere intersects the specified frustum; false otherwise.
	 */
	bool Intersects(const Frustum& frustum) const;

	/**
	 * Tests whether this bounding box intersects the specified plane.
	 *
	 * @param plane The plane to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if this bounding box is in the negative half-space of
	 *  the plane, Plane::kIntersectionFront if it is in the positive half-space of the plane;
	 *  and Plane::kIntersectionExists if it intersects the plane.
	 */
	int Intersects(const Plane& plane) const;

	/**
	 * Tests whether this bounding box intersects the specified ray.
	 *
	 * @param[in] ray The ray to test intersection with.
	 * @param[opt] distance The distance from the origin of the ray to this bounding box.
	 * 
	 * @return True if this ray intersects the bounding object and false otherwise.
	 */
	bool Intersects(const Ray& ray, float* distance) const;

	/**
	 * Determines if this bounding box is empty.
	 *
	 * @return true if this bounding box is empty; false otherwise.
	 */
	bool IsEmpty() const;

	/**
	 * Sets this bounding box to the smallest bounding box
	 * that contains both this bounding box and the specified bounding sphere.
	 *
	 * @param sphere The bounding sphere to merge with.
	 */
	void Merge(const BoundingSphere& sphere);

	/**
	 * Sets this bounding box to the smallest bounding box
	 * that contains both this bounding object and the specified bounding box.
	 *
	 * @param box The bounding box to merge with.
	 */
	void Merge(const BoundingBox& box);

	/**
	 * Scales this bounding box by a value.
	 *
	 * @param scale The scalar scale parameter.
	 */
	void Scale(float scale);

	/**
	 * Scales this bounding box by a value.
	 *
	 * @param scale The vector scale parameter.
	 */
	void Scale(const Vector3& scale);

	/**
	 * Sets this bounding box to the specified values.
	 *
	 * @param min The minimum point of the bounding box.
	 * @param max The maximum point of the bounding box.
	 */
	void Set(const Vector3& min, const Vector3& max);

	/**
	 * Sets this bounding box to the specified values.
	 * 
	 * @param minX The x coordinate of the minimum point of the bounding box.
	 * @param minY The y coordinate of the minimum point of the bounding box.
	 * @param minZ The z coordinate of the minimum point of the bounding box.
	 * @param maxX The x coordinate of the maximum point of the bounding box.
	 * @param maxY The y coordinate of the maximum point of the bounding box.
	 * @param maxZ The z coordinate of the maximum point of the bounding box.
	 */
	void Set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	/**
	 * Sets this bounding box to the specified bounding box.
	 *
	 * @param box The bounding box to set to.
	 */
	void Set(const BoundingBox& box);

	/**
	 * Sets this box to tightly contain the specified bounding sphere.
	 *
	 * @param sphere The sphere to contain.
	 */
	void Set(const BoundingSphere& sphere);

	/**
	 * Transforms the bounding box by the given transformation matrix.
	 *
	 * @param matrix The transformation matrix to transform by.
	 */
	void Transform(const Matrix4& matrix);


	/**
	 * Prepares for calculation (sets min and max to infinite values).
	 */
	void Prepare();

	/**
	 * Adds point to bounding box for calculation.
	 *
	 * @param point The point to be added.
	 */
	void AddPoint(const Vector3& point);

	/**
	 * Transforms this bounding box by the given matrix.
	 * 
	 * @param matrix The matrix to transform by.
	 * @return This bounding box, after the transformation occurs.
	 */
	inline BoundingBox& operator*=(const Matrix4& matrix);
};

/**
 * Transforms the given bounding box by the given matrix.
 * 
 * @param matrix The matrix to transform by.
 * @param box The bounding box to transform.
 * @return The resulting transformed bounding box.
 */
inline const BoundingBox operator*(const Matrix4& matrix, const BoundingBox& box);

} // namespace scythe

#include "bounding_box.inl"

#endif