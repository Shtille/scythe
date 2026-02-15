#include <scythe/math/ray.h>

#include <cmath>

#include <scythe/math/plane.h>
#include <scythe/math/frustum.h>
#include <scythe/math/bounding_sphere.h>
#include <scythe/math/bounding_box.h>
#include <scythe/math/constants.h>
#include <scythe/math/common.h>
#include <scythe/math/matrix4.h>
#include <scythe/defines.h>

namespace scythe {

	Ray::Ray()
	: direction_(0.0f, 0.0f, 1.0f)
	{
	}

	Ray::Ray(const Vector3& origin, const Vector3& direction)
	{
		Set(origin, direction);
	}

	Ray::Ray(float originX, float originY, float originZ, float dirX, float dirY, float dirZ)
	{
		Set(Vector3(originX, originY, originZ), Vector3(dirX, dirY, dirZ));
	}

	Ray::Ray(const Ray& copy)
	{
		Set(copy);
	}

	Ray::~Ray()
	{
	}

	const Vector3& Ray::GetOrigin() const
	{
		return origin_;
	}

	void Ray::SetOrigin(const Vector3& origin)
	{
		origin_ = origin;
	}

	void Ray::SetOrigin(float x, float y, float z)
	{
		origin_.Set(x, y, z);
	}

	const Vector3& Ray::GetDirection() const
	{
		return direction_;
	}

	void Ray::SetDirection(const Vector3& direction)
	{
		direction_ = direction;
		Normalize();
	}

	void Ray::SetDirection(float x, float y, float z)
	{
		direction_.Set(x, y, z);
		Normalize();
	}

	bool Ray::Intersects(const BoundingSphere& sphere, float* distance) const
	{
		return sphere.Intersects(*this, distance);
	}

	bool Ray::Intersects(const BoundingBox& box, float* distance) const
	{
		return box.Intersects(*this, distance);
	}

	bool Ray::Intersects(const Frustum& frustum, float* distance) const
	{
		Plane n = frustum.GetNear();
		float nD = -1.0f;
		(void)Intersects(n, &nD);
		float nOD = n.Distance(origin_);

		Plane f = frustum.GetFar();
		float fD = -1.0f;
		(void)Intersects(f, &fD);
		float fOD = f.Distance(origin_);

		Plane l = frustum.GetLeft();
		float lD = -1.0f;
		(void)Intersects(l, &lD);
		float lOD = l.Distance(origin_);

		Plane r = frustum.GetRight();
		float rD = -1.0f;
		(void)Intersects(r, &rD);
		float rOD = r.Distance(origin_);

		Plane b = frustum.GetBottom();
		float bD = -1.0f;
		(void)Intersects(b, &bD);
		float bOD = b.Distance(origin_);

		Plane t = frustum.GetTop();
		float tD = -1.0f;
		(void)Intersects(t, &tD);
		float tOD = t.Distance(origin_);

		// If the ray's origin is in the negative half-space of one of the frustum's planes
		// and it does not intersect that same plane, then it does not intersect the frustum.
		if ((nOD < 0.0f && nD < 0.0f) || (fOD < 0.0f && fD < 0.0f) ||
			(lOD < 0.0f && lD < 0.0f) || (rOD < 0.0f && rD < 0.0f) ||
			(bOD < 0.0f && bD < 0.0f) || (tOD < 0.0f && tD < 0.0f))
		{
			return false;
		}

		// Otherwise, the intersection distance is the minimum positive intersection distance.
		if (distance)
		{
			float d = (nD > 0.0f) ? nD : 0.0f;
			d = (fD > 0.0f) ? ((d == 0.0f) ? fD : Min(fD, d)) : d;
			d = (lD > 0.0f) ? ((d == 0.0f) ? lD : Min(lD, d)) : d;
			d = (rD > 0.0f) ? ((d == 0.0f) ? rD : Min(rD, d)) : d;
			d = (tD > 0.0f) ? ((d == 0.0f) ? bD : Min(bD, d)) : d;
			d = (bD > 0.0f) ? ((d == 0.0f) ? tD : Min(tD, d)) : d;

			*distance = d;
		}
		return true;
	}

	bool Ray::Intersects(const Plane& plane, float* distance) const
	{
		const Vector3& normal = plane.GetNormal();
		// If the origin of the ray is on the plane then the distance is zero.
		float alpha = (normal.Dot(origin_) + plane.GetDistance());
		if (fabs(alpha) < kFloatEpsilon)
		{
			if (distance)
				*distance = 0.0f;
			return true;
		}

		float dot = normal.Dot(direction_);
		
		// If the dot product of the plane's normal and this ray's direction is zero,
		// then the ray is parallel to the plane and does not intersect it.
		if (dot == 0.0f)
		{
			return false;
		}
		
		// Calculate the distance along the ray's direction vector to the point where
		// the ray intersects the plane (if it is negative the plane is behind the ray).
		float d = -alpha / dot;
		if (d < 0.0f)
		{
			return false;
		}
		if (distance)
			*distance = d;
		return true;
	}

	void Ray::Set(const Vector3& origin, const Vector3& direction)
	{
		origin_ = origin;
		direction_ = direction;
		Normalize();
	}

	void Ray::Set(const Ray& ray)
	{
		origin_ = ray.origin_;
		direction_ = ray.direction_;
		Normalize();
	}

	void Ray::Transform(const Matrix4& matrix)
	{
		matrix.TransformPoint(&origin_);
		matrix.TransformVector(&direction_);
		direction_.Normalize();
	}

	void Ray::Normalize()
	{
		if (direction_.IsZero())
		{
			//SC_ERROR("Invalid ray object; a ray's direction must be non-zero.");
			return;
		}

		// Normalize the ray's direction vector.
		float normalizeFactor = 1.0f / sqrt(direction_.x * direction_.x + direction_.y * direction_.y + direction_.z * direction_.z);
		if (normalizeFactor != 1.0f)
		{
			direction_.x *= normalizeFactor;
			direction_.y *= normalizeFactor;
			direction_.z *= normalizeFactor;
		}
	}

} // namespace scythe