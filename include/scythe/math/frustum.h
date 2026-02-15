#ifndef __SCYTHE_FRUSTUM_H__
#define __SCYTHE_FRUSTUM_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include "plane.h"
#include "matrix4.h"

namespace scythe {

class Ray;
class BoundingSphere;
class BoundingBox;

/**
 * Defines a 3-dimensional frustum.
 *
 * A frustum in computer graphics is generally a volume of 3D space,
 * defined as the part of a rectangular pyramid that lies between
 * two planes perpendicular to its center line. A frustum is often used
 * to represent what a "camera" sees in your 3D space.
 *
 * The Frustum class allows you to define a bounding frustum using a combined
 * matrix that is generally the product of a view matrix and a projection matrix.
 *
 * You can query a Frustum object for any one of its bounding planes,
 * for its corners, and for whether it intersects with a given object.
 * Since objects that don't intersect with your view frustum generally
 * don't need to be rendered, culling them quickly can save you a lot of
 * rendering time.
 */
class Frustum
{
public:

	/**
	 * Constructs the default frustum (corresponds to the identity matrix).
	 */
	Frustum();

	/**
	 * Constructs a new frustum from the specified view projection matrix.
	 *
	 * @param matrix The view projection matrix to create this frustum from.
	 */
	Frustum(const Matrix4& matrix);

	/**
	 * Constructs a new frustum from the given frustum.
	 * 
	 * @param frustum The frustum to create this frustum from.
	 */
	Frustum(const Frustum& frustum);

	/**
	 * Destructor.
	 */
	~Frustum();

	/**
	 * Gets the near plane of the frustum.
	 *
	 * @return near The near plane.
	 */
	const Plane& GetNear() const;

	/**
	 * Gets the far plane of the frustum.
	 *
	 * @return far The far plane.
	 */
	const Plane& GetFar() const;

	/**
	 * Gets the left plane of the frustum.
	 *
	 * @return left The left plane.
	 */
	const Plane& GetLeft() const;

	/**
	 * Gets the right plane of the frustum.
	 *
	 * @return right The right plane.
	 */
	const Plane& GetRight() const;

	/**
	 * Gets the bottom plane of the frustum.
	 *
	 * @return bottom The bottom plane.
	 */
	const Plane& GetBottom() const;

	/**
	 * Gets the top plane of the frustum.
	 *
	 * @return top The top plane.
	 */
	const Plane& GetTop() const;

	/**
	 * Gets the projection matrix corresponding to the frustum in the specified matrix.
	 * 
	 * @param dst The projection matrix to copy into.
	 */
	void GetMatrix(Matrix4* dst) const;

	/**
	 * Gets the corners of the frustum in the specified array.
	 *
	 * The corners are stored in the following order:
	 * (N-near, F-far, L-left, R-right, B-bottom, T-top)
	 * LTN, LBN, RBN, RTN, RTF, RBF, LBF, LTF.
	 * 
	 * @param corners The array (of at least size 8) to store the corners in.
	 */
	void GetCorners(Vector3* corners) const;

	/**
	 * Gets the corners of the frustum's near plane in the specified array.
	 *
	 * The corners are stored in the following order:
	 * left-top, left-bottom, right-bottom, right-top.
	 *
	 * @param corners The array (of at least size 4) to store the corners in.
	 */
	void GetNearCorners(Vector3* corners) const;

	/**
	 * Gets the corners of the frustum's far plane in the specified array.
	 *
	 * The corners are stored in the following order:
	 * right-top, right-bottom, left-bottom, left-top.
	 *
	 * @param corners The array (of at least size 4) to store the corners in.
	 */
	void GetFarCorners(Vector3* corners) const;

	/**
	 * Tests whether this frustum intersects the specified point.
	 *
	 * @param point The point to test intersection with.
	 *
	 * @return true if the specified point intersects this frustum; false otherwise.
	 */
	bool Intersects(const Vector3& point) const;

	/**
	 * Tests whether this frustum intersects the specified point.
	 *
	 * @param x The x coordinate.
	 * @param y The y coordinate.
	 * @param z The z coordinate.
	 *
	 * @return true if the specified point intersects this frustum; false otherwise.
	 */
	bool Intersects(float x, float y, float z) const;

	/**
	 * Tests whether this frustum intersects the specified bounding sphere.
	 *
	 * @param sphere The bounding sphere to test intersection with.
	 * 
	 * @return true if the specified bounding sphere intersects this frustum; false otherwise.
	 */
	bool Intersects(const BoundingSphere& sphere) const;

	/**
	 * Tests whether this frustum intersects the specified bounding box.
	 *
	 * @param box The bounding box to test intersection with.
	 * 
	 * @return true if the specified bounding box intersects this frustum; false otherwise.
	 */
	bool Intersects(const BoundingBox& box) const;

	/**
	 * Tests whether this frustum intersects the specified plane.
	 *
	 * @param plane The plane to test intersection with.
	 * 
	 * @return Plane::kIntersectionBack if the specified bounding object is in the negative half-space of
	 *  this plane, Plane::kIntersectionFront if it is in the positive half-space of this plane,
	 *  and Plane::kIntersectionExists if it intersects this plane.
	 */
	int Intersects(const Plane& plane) const;

	/**
	 * Tests whether this frustum intersects the specified ray.
	 *
	 * @param[in] ray The ray to test intersection with.
	 * @param[opt,out] distance The distance from the origin of this ray to the frustum.
	 * 
	 * @return True if this ray intersects the bounding object and false otherwise.
	 */
	bool Intersects(const Ray& ray, float* distance) const;

	/**
	 * Sets this frustum to the specified frustum.
	 *
	 * @param frustum The frustum to set to.
	 */
	void Set(const Frustum& frustum);

	/**
	 * Sets the frustum to the frustum corresponding to the specified view projection matrix.
	 *
	 * @param matrix The view projection matrix.
	 */
	void Set(const Matrix4& matrix);

private:

	/**
	 * Updates the planes of the frustum.
	 */
	void UpdatePlanes();

	Plane near_;
	Plane far_;
	Plane bottom_;
	Plane top_;
	Plane left_;
	Plane right_;
	Matrix4 matrix_;
};

} // namespace scythe

#endif