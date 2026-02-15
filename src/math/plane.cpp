#include <scythe/math/plane.h>

#include <cmath>

#include <scythe/math/ray.h>
#include <scythe/math/frustum.h>
#include <scythe/math/bounding_sphere.h>
#include <scythe/math/bounding_box.h>
#include <scythe/math/constants.h>
#include <scythe/math/matrix4.h>
#include <scythe/defines.h>

namespace scythe {

	Plane::Plane()
	: normal_(0.0f, 1.0f, 0.0f)
	, distance_(0.0f)
	{
	}

	Plane::Plane(const Vector3& normal, float distance)
	{
		Set(normal, distance);
	}

	Plane::Plane(float normalX, float normalY, float normalZ, float distance)
	{
		Set(Vector3(normalX, normalY, normalZ), distance);
	}

	Plane::Plane(const Plane& copy)
	{
		Set(copy);
	}

	Plane::~Plane()
	{
	}

	const Vector3& Plane::GetNormal() const
	{
		return normal_;
	}

	void Plane::SetNormal(const Vector3& normal)
	{
		normal_ = normal;
		Normalize();
	}

	void Plane::SetNormal(float x, float y, float z)
	{
		normal_.Set(x, y, z);
		Normalize();
	}

	float Plane::GetDistance() const
	{
		return distance_;
	}

	void Plane::SetDistance(float distance)
	{
		distance_ = distance;
	}

	float Plane::Distance(const Vector3& point) const
	{
		return normal_.x * point.x + normal_.y * point.y + normal_.z * point.z + distance_;
	}

	void Plane::Intersection(const Plane& p1, const Plane& p2, const Plane& p3, Vector3* point)
	{
		SCYTHE_ASSERT(point);

		// The planes' normals must be all normalized (which we guarantee in the Plane class).
		// Calculate the determinant of the matrix (i.e | n1 n2 n3 |).
		float det = p1.normal_.x * (p2.normal_.y * p3.normal_.z -
					p2.normal_.z * p3.normal_.y) - p2.normal_.x *(p1.normal_.y * p3.normal_.z -
					p1.normal_.z * p3.normal_.y) + p3.normal_.x * (p1.normal_.y * p2.normal_.z - p1.normal_.z * p2.normal_.y);

		// If the determinant is zero, then the planes do not all intersect.
		if (fabs(det) <= kFloatEpsilon)
			return;

		// Create 3 points, one on each plane.
		// (We just pick the point on the plane directly along its normal from the origin).
		float p1x = -p1.normal_.x * p1.distance_;
		float p1y = -p1.normal_.y * p1.distance_;
		float p1z = -p1.normal_.z * p1.distance_;
		float p2x = -p2.normal_.x * p2.distance_;
		float p2y = -p2.normal_.y * p2.distance_;
		float p2z = -p2.normal_.z * p2.distance_;
		float p3x = -p3.normal_.x * p3.distance_;
		float p3y = -p3.normal_.y * p3.distance_;
		float p3z = -p3.normal_.z * p3.distance_;

		// Calculate the cross products of the normals.
		float c1x = (p2.normal_.y * p3.normal_.z) - (p2.normal_.z * p3.normal_.y);
		float c1y = (p2.normal_.z * p3.normal_.x) - (p2.normal_.x * p3.normal_.z);
		float c1z = (p2.normal_.x * p3.normal_.y) - (p2.normal_.y * p3.normal_.x);
		float c2x = (p3.normal_.y * p1.normal_.z) - (p3.normal_.z * p1.normal_.y);
		float c2y = (p3.normal_.z * p1.normal_.x) - (p3.normal_.x * p1.normal_.z);
		float c2z = (p3.normal_.x * p1.normal_.y) - (p3.normal_.y * p1.normal_.x);
		float c3x = (p1.normal_.y * p2.normal_.z) - (p1.normal_.z * p2.normal_.y);
		float c3y = (p1.normal_.z * p2.normal_.x) - (p1.normal_.x * p2.normal_.z);
		float c3z = (p1.normal_.x * p2.normal_.y) - (p1.normal_.y * p2.normal_.x);

		// Calculate the point of intersection using the formula:
		// x = (| n1 n2 n3 |)^-1 * [(x1 * n1)(n2 x n3) + (x2 * n2)(n3 x n1) + (x3 * n3)(n1 x n2)]
		float s1 = p1x * p1.normal_.x + p1y * p1.normal_.y + p1z * p1.normal_.z;
		float s2 = p2x * p2.normal_.x + p2y * p2.normal_.y + p2z * p2.normal_.z;
		float s3 = p3x * p3.normal_.x + p3y * p3.normal_.y + p3z * p3.normal_.z;
		float detI = 1.0f / det;
		point->x = (s1 * c1x + s2 * c2x + s3 * c3x) * detI;
		point->y = (s1 * c1y + s2 * c2y + s3 * c3y) * detI;
		point->z = (s1 * c1z + s2 * c2z + s3 * c3z) * detI;
	}

	int Plane::Intersects(const BoundingSphere& sphere) const
	{
		return sphere.Intersects(*this);
	}

	int Plane::Intersects(const BoundingBox& box) const
	{
		return box.Intersects(*this);
	}

	int Plane::Intersects(const Frustum& frustum) const
	{
		// Get the corners of the frustum.
		Vector3 corners[8];
		frustum.GetCorners(corners);

		// Calculate the distances from all of the corners to the plane.
		// If all of the distances are positive, then the frustum is in the
		// positive half-space of this plane; if all the distances are negative,
		// then the frustum is in the negative half-space of this plane; if some of
		// the distances are positive and some are negative, the frustum intersects.
		float d = Distance(corners[0]);
		if (d > 0.0f)
		{
			if (Distance(corners[1]) <= 0.0f ||
				Distance(corners[2]) <= 0.0f ||
				Distance(corners[3]) <= 0.0f ||
				Distance(corners[4]) <= 0.0f ||
				Distance(corners[5]) <= 0.0f ||
				Distance(corners[6]) <= 0.0f ||
				Distance(corners[7]) <= 0.0f)
			{
				return Plane::kIntersectionExists;
			}

			return Plane::kIntersectionFront;
		}
		else if (d < 0.0f)
		{
			if (Distance(corners[1]) >= 0.0f ||
				Distance(corners[2]) >= 0.0f ||
				Distance(corners[3]) >= 0.0f ||
				Distance(corners[4]) >= 0.0f ||
				Distance(corners[5]) >= 0.0f ||
				Distance(corners[6]) >= 0.0f ||
				Distance(corners[7]) >= 0.0f)
			{
				return Plane::kIntersectionExists;
			}

			return Plane::kIntersectionBack;
		}
		else
		{
			return Plane::kIntersectionExists;
		}
	}

	int Plane::Intersects(const Plane& plane) const
	{
		// Check if the planes intersect.
		if ((normal_.x == plane.normal_.x && normal_.y == plane.normal_.y && normal_.z == plane.normal_.z) || !IsParallel(plane))
		{
			return Plane::kIntersectionExists;
		}

		// Calculate the point where the given plane's normal vector intersects the given plane.
		Vector3 point(plane.normal_.x * -plane.distance_, plane.normal_.y * -plane.distance_, plane.normal_.z * -plane.distance_);

		// Calculate whether the given plane is in the positive or negative half-space of this plane
		// (corresponds directly to the sign of the distance from the point calculated above to this plane).
		if (Distance(point) > 0.0f)
		{
			return Plane::kIntersectionFront;
		}
		else
		{
			return Plane::kIntersectionBack;
		}
	}

	int Plane::Intersects(const Ray& ray) const
	{
		// Calculate the distance from the ray's origin to the plane.
		float d = Distance(ray.GetOrigin());

		// If the origin of the ray lies in the plane, then it intersects.
		if (d == 0.0f)
		{
			return Plane::kIntersectionExists;
		}
		else
		{
			Vector3 rayDirection = ray.GetDirection();
			// If the dot product of this plane's normal and the ray's direction is positive, and
			// if the distance from this plane to the ray's origin is negative -> intersection, OR
			// if the dot product of this plane's normal and the ray's direction is negative, and
			// if the distance from this plane to the ray's origin is positive -> intersection.
			if (normal_.x * rayDirection.x + normal_.y * rayDirection.y + normal_.z * rayDirection.z > 0.0f)
			{
				if (d < 0.0f)
				{
					return Plane::kIntersectionExists;
				}
				else
				{
					return Plane::kIntersectionFront;
				}
			}
			else
			{
				if (d > 0.0f)
				{
					return Plane::kIntersectionExists;
				}
				else
				{
					return Plane::kIntersectionBack;
				}
			}
		}
	}

	bool Plane::IsParallel(const Plane& plane) const
	{
		return (normal_.y * plane.normal_.z) - (normal_.z * plane.normal_.y) == 0.0f &&
			   (normal_.z * plane.normal_.x) - (normal_.x * plane.normal_.z) == 0.0f &&
			   (normal_.x * plane.normal_.y) - (normal_.y * plane.normal_.x) == 0.0f;
	}

	void Plane::Set(const Vector3& normal, float distance)
	{
		normal_ = normal;
		distance_ = distance;
		Normalize();
	}

	void Plane::Set(const Plane& plane)
	{
		normal_ = plane.normal_;
		distance_ = plane.distance_;
	}

	void Plane::Transform(const Matrix4& matrix)
	{
		Matrix4 inverted;
		if (matrix.Invert(&inverted))
		{
			// Treat the plane as a four-tuple and multiply by the inverse transpose of the matrix to get the transformed plane.
			// Then we normalize the plane by dividing both the normal and the distance by the length of the normal.
			float nx = normal_.x * inverted.m[0] + normal_.y * inverted.m[1] + normal_.z * inverted.m[2] + distance_ * inverted.m[3];
			float ny = normal_.x * inverted.m[4] + normal_.y * inverted.m[5] + normal_.z * inverted.m[6] + distance_ * inverted.m[7];
			float nz = normal_.x * inverted.m[8] + normal_.y * inverted.m[9] + normal_.z * inverted.m[10] + distance_ * inverted.m[11];
			float d = normal_.x * inverted.m[12]+ normal_.y * inverted.m[13] + normal_.z * inverted.m[14] + distance_ * inverted.m[15];
			float divisor = sqrt(nx * nx + ny * ny + nz * nz);
			SCYTHE_ASSERT(divisor);
			float factor = 1.0f / divisor;

			normal_.x = nx * factor;
			normal_.y = ny * factor;
			normal_.z = nz * factor;
			distance_ = d * factor;
		}
	}

	void Plane::Normalize()
	{
		if (normal_.IsZero())
			return;

		// Normalize the plane's normal.
		float normalizeFactor = 1.0f / sqrt(normal_.x * normal_.x + normal_.y * normal_.y + normal_.z * normal_.z);

		if (normalizeFactor != 1.0f)
		{
			normal_.x *= normalizeFactor;
			normal_.y *= normalizeFactor;
			normal_.z *= normalizeFactor;
			distance_ *= normalizeFactor;
		}
	}

} // namespace scythe