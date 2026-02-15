#include <scythe/math/frustum.h>

#include <scythe/math/ray.h>
#include <scythe/math/bounding_sphere.h>
#include <scythe/math/bounding_box.h>
#include <scythe/math/constants.h>
#include <scythe/defines.h>

namespace scythe {

	Frustum::Frustum()
	{
		Set(Matrix4::Identity());
	}

	Frustum::Frustum(const Matrix4& matrix)
	{
		Set(matrix);
	}

	Frustum::Frustum(const Frustum& frustum)
	{
		Set(frustum);
	}

	Frustum::~Frustum()
	{
	}

	const Plane& Frustum::GetNear() const
	{
		return near_;
	}

	const Plane& Frustum::GetFar() const
	{
		return far_;
	}

	const Plane& Frustum::GetLeft() const
	{
		return left_;
	}

	const Plane& Frustum::GetRight() const
	{
		return right_;
	}

	const Plane& Frustum::GetBottom() const
	{
		return bottom_;
	}

	const Plane& Frustum::GetTop() const
	{
		return top_;
	}

	void Frustum::GetMatrix(Matrix4* dst) const
	{
		SCYTHE_ASSERT(dst);
		dst->Set(matrix_);
	}

	void Frustum::GetCorners(Vector3* corners) const
	{
		GetNearCorners(corners);
		GetFarCorners(corners + 4);
	}

	void Frustum::GetNearCorners(Vector3* corners) const
	{
		SCYTHE_ASSERT(corners);

		Plane::Intersection(near_, left_, top_, &corners[0]);
		Plane::Intersection(near_, left_, bottom_, &corners[1]);
		Plane::Intersection(near_, right_, bottom_, &corners[2]);
		Plane::Intersection(near_, right_, top_, &corners[3]);
	}

	void Frustum::GetFarCorners(Vector3* corners) const
	{
		SCYTHE_ASSERT(corners);

		Plane::Intersection(far_, right_, top_, &corners[0]);
		Plane::Intersection(far_, right_, bottom_, &corners[1]);
		Plane::Intersection(far_, left_, bottom_, &corners[2]);
		Plane::Intersection(far_, left_, top_, &corners[3]);
	}

	bool Frustum::Intersects(const Vector3& point) const
	{
		if (near_.Distance(point) <= 0.0f)
			return false;
		if (far_.Distance(point) <= 0.0f)
			return false;
		if (left_.Distance(point) <= 0.0f)
			return false;
		if (right_.Distance(point) <= 0.0f)
			return false;
		if (top_.Distance(point) <= 0.0f)
			return false;
		if (bottom_.Distance(point) <= 0.0f)
			return false;

		return true;
	}

	bool Frustum::Intersects(float x, float y, float z) const
	{
		return Intersects(Vector3(x, y, z));
	}

	bool Frustum::Intersects(const BoundingSphere& sphere) const
	{
		return sphere.Intersects(*this);
	}

	bool Frustum::Intersects(const BoundingBox& box) const
	{
		return box.Intersects(*this);
	}

	int Frustum::Intersects(const Plane& plane) const
	{
		return plane.Intersects(*this);
	}

	bool Frustum::Intersects(const Ray& ray, float* distance) const
	{
		return ray.Intersects(*this, distance);
	}

	void Frustum::Set(const Frustum& frustum)
	{
		near_ = frustum.near_;
		far_ = frustum.far_;
		bottom_ = frustum.bottom_;
		top_ = frustum.top_;
		left_ = frustum.left_;
		right_ = frustum.right_;
		matrix_.Set(frustum.matrix_);
	}

	void Frustum::UpdatePlanes()
	{
		near_.Set(Vector3(matrix_.m[3] + matrix_.m[2], matrix_.m[7] + matrix_.m[6], matrix_.m[11] + matrix_.m[10]), matrix_.m[15] + matrix_.m[14]);
		far_.Set(Vector3(matrix_.m[3] - matrix_.m[2], matrix_.m[7] - matrix_.m[6], matrix_.m[11] - matrix_.m[10]), matrix_.m[15] - matrix_.m[14]);
		bottom_.Set(Vector3(matrix_.m[3] + matrix_.m[1], matrix_.m[7] + matrix_.m[5], matrix_.m[11] + matrix_.m[9]), matrix_.m[15] + matrix_.m[13]);
		top_.Set(Vector3(matrix_.m[3] - matrix_.m[1], matrix_.m[7] - matrix_.m[5], matrix_.m[11] - matrix_.m[9]), matrix_.m[15] - matrix_.m[13]);
		left_.Set(Vector3(matrix_.m[3] + matrix_.m[0], matrix_.m[7] + matrix_.m[4], matrix_.m[11] + matrix_.m[8]), matrix_.m[15] + matrix_.m[12]);
		right_.Set(Vector3(matrix_.m[3] - matrix_.m[0], matrix_.m[7] - matrix_.m[4], matrix_.m[11] - matrix_.m[8]), matrix_.m[15] - matrix_.m[12]);
	}

	void Frustum::Set(const Matrix4& matrix)
	{
		matrix_.Set(matrix);

		// Update the planes.
		UpdatePlanes();
	}

} // namespace scythe