#include <scythe/math/bounding_sphere.h>

#include <cmath>

#include <scythe/math/ray.h>
#include <scythe/math/plane.h>
#include <scythe/math/frustum.h>
#include <scythe/math/bounding_box.h>
#include <scythe/math/constants.h>
#include <scythe/math/common.h>
#include <scythe/math/matrix4.h>
#include <scythe/defines.h>

namespace scythe {

	BoundingSphere::BoundingSphere()
	: radius(0.0f)
	{
	}

	BoundingSphere::BoundingSphere(const Vector3& center, float radius)
	{
		Set(center, radius);
	}

	BoundingSphere::BoundingSphere(const BoundingSphere& copy)
	{
		Set(copy);
	}

	BoundingSphere::~BoundingSphere()
	{
	}

	const BoundingSphere& BoundingSphere::Empty()
	{
		static BoundingSphere s;
		return s;
	}

	bool BoundingSphere::Intersects(const BoundingSphere& sphere) const
	{
		// If the distance between the spheres' centers is less than or equal
		// to the sum of their radii, then the spheres intersect.
		float vx = sphere.center.x - center.x;
		float vy = sphere.center.y - center.y;
		float vz = sphere.center.z - center.z;

		return sqrt(vx * vx + vy * vy + vz * vz) <= (radius + sphere.radius);
	}

	bool BoundingSphere::Intersects(const BoundingBox& box) const
	{
		// Determine what point is closest; if the distance to that
		// point is less than the radius, then this sphere intersects.
		float cpX = center.x;
		float cpY = center.y;
		float cpZ = center.z;

		const Vector3& boxMin = box.min;
		const Vector3& boxMax = box.max;
		// Closest x value.
		if (center.x < boxMin.x)
		{
			cpX = boxMin.x;
		}
		else if (center.x > boxMax.x)
		{
			cpX = boxMax.x;
		}

		// Closest y value.
		if (center.y < boxMin.y)
		{
			cpY = boxMin.y;
		}
		else if (center.y > boxMax.y)
		{
			cpY = boxMax.y;
		}

		// Closest z value.
		if (center.z < boxMin.z)
		{
			cpZ = boxMin.z;
		}
		else if (center.z > boxMax.z)
		{
			cpZ = boxMax.z;
		}

		// Find the distance to the closest point and see if it is less than or equal to the radius.
		cpX -= center.x;
		cpY -= center.y;
		cpZ -= center.z;

		return sqrtf(cpX * cpX + cpY * cpY + cpZ * cpZ) <= radius;
	}

	bool BoundingSphere::Intersects(const Frustum& frustum) const
	{
		// The sphere must either intersect or be in the positive half-space of all six planes of the frustum.
		return (Intersects(frustum.GetNear()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetFar()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetLeft()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetRight()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetBottom()) != Plane::kIntersectionBack &&
				Intersects(frustum.GetTop()) 	!= Plane::kIntersectionBack );
	}

	int BoundingSphere::Intersects(const Plane& plane) const
	{
		float distance = plane.Distance(center);

		if (fabsf(distance) <= radius)
		{
			return Plane::kIntersectionExists;
		}
		else if (distance > 0.0f)
		{
			return Plane::kIntersectionFront;
		}
		else
		{
			return Plane::kIntersectionBack;
		}
	}

	bool BoundingSphere::Intersects(const Ray& ray, float* distance) const
	{
		const Vector3& origin = ray.GetOrigin();
		const Vector3& direction = ray.GetDirection();

		// Calculate the vector and the square of the distance from the ray's origin to this sphere's center.
		float vx = origin.x - center.x;
		float vy = origin.y - center.y;
		float vz = origin.z - center.z;
		float d2 = vx * vx + vy * vy + vz * vz;

		// Solve the quadratic equation using the ray's and sphere's equations together.
		// Since the ray's direction is guaranteed to be 1 by the Ray, we don't need to
		// calculate and use A (A=ray.getDirection().lengthSquared()).
		float B = 2.0f * (vx * direction.x + vy * direction.y + vz * direction.z);
		float C = d2 - radius * radius;
		float discriminant = B * B - 4.0f * C;

		// If the discriminant is negative, then there is no intersection.
		if (discriminant < 0.0f)
		{
			return false;
		}
		else
		{
			// The intersection is at the smaller positive root.
			float sqrtDisc = sqrtf(discriminant);
			float t0 = (-B - sqrtDisc) * 0.5f;
			float t1 = (-B + sqrtDisc) * 0.5f;
			if (distance)
				*distance = (t0 > 0.0f && t0 < t1) ? t0 : t1;
			return true;
		}
	}

	bool BoundingSphere::IsEmpty() const
	{
		return radius == 0.0f;
	}

	void BoundingSphere::Merge(const BoundingSphere& sphere)
	{
		if (sphere.IsEmpty())
			return;

		// Calculate the distance between the two centers.
		float vx = center.x - sphere.center.x;
		float vy = center.y - sphere.center.y;
		float vz = center.z - sphere.center.z;
		float d = sqrtf(vx * vx + vy * vy + vz * vz);

		// If one sphere is contained inside the other, set to the larger sphere.
		if (d <= (sphere.radius - radius))
		{
			center = sphere.center;
			radius = sphere.radius;
			return;
		}
		else if (d <= (radius - sphere.radius))
		{
			return;
		}

		// Calculate the unit vector between the two centers.
		SCYTHE_ASSERT(d != 0.0f);
		float dI = 1.0f / d;
		vx *= dI;
		vy *= dI;
		vz *= dI;

		// Calculate the new radius.
		float r = (radius + sphere.radius + d) * 0.5f;

		// Calculate the new center.
		float scaleFactor = (r - sphere.radius);
		vx = vx * scaleFactor + sphere.center.x;
		vy = vy * scaleFactor + sphere.center.y;
		vz = vz * scaleFactor + sphere.center.z;

		// Set the new center and radius.
		center.x = vx;
		center.y = vy;
		center.z = vz;
		radius = r;
	}

	void BoundingSphere::Merge(const BoundingBox& box)
	{
		if (box.IsEmpty())
			return;

		const Vector3& min = box.min;
		const Vector3& max = box.max;

		// Find the corner of the bounding box that is farthest away from this sphere's center.
		float v1x = min.x - center.x;
		float v1y = min.y - center.y;
		float v1z = min.z - center.z;
		float v2x = max.x - center.x;
		float v2y = max.y - center.y;
		float v2z = max.z - center.z;
		float fx = min.x;
		float fy = min.y;
		float fz = min.z;

		if (v2x > v1x)
		{
			fx = max.x;
		}
		if (v2y > v1y)
		{
			fy = max.y;
		}
		if (v2z > v1z)
		{
			fz = max.z;
		}

		// Calculate the unit vector and the distance between the center and the farthest point.
		v1x = center.x - fx;
		v1y = center.y - fy;
		v1z = center.z - fz;
		float distance = sqrtf(v1x * v1x + v1y * v1y + v1z * v1z);

		// If the box is inside the sphere, we are done.
		if (distance <= radius)
		{
			return;
		}

		// Calculate the unit vector between the center and the farthest point.
		SCYTHE_ASSERT(distance != 0.0f);
		float dI = 1.0f / distance;
		v1x *= dI;
		v1y *= dI;
		v1z *= dI;

		// Calculate the new radius.
		float r = (radius + distance) * 0.5f;

		// Calculate the new center.
		v1x = v1x * r + fx;
		v1y = v1y * r + fy;
		v1z = v1z * r + fz;

		// Set the new center and radius.
		center.x = v1x;
		center.y = v1y;
		center.z = v1z;
		radius = r;
	}

	void BoundingSphere::Set(const Vector3& center, float radius)
	{
		this->center = center;
		this->radius = radius;
	}

	void BoundingSphere::Set(const BoundingSphere& sphere)
	{
		center = sphere.center;
		radius = sphere.radius;
	}

	void BoundingSphere::Set(const BoundingBox& box)
	{
		center.x = (box.min.x + box.max.x) * 0.5f;
		center.y = (box.min.y + box.max.y) * 0.5f;
		center.z = (box.min.z + box.max.z) * 0.5f;
		radius = center.Distance(box.max);
	}

	void BoundingSphere::Transform(const Matrix4& matrix)
	{
		// Translate the center point.
		matrix.TransformPoint(center, &center);

		// Scale the sphere's radius by the scale fo the matrix
		Vector3 scale;
		matrix.Decompose(&scale, nullptr, nullptr);
		float r = radius * scale.x;
		r = Max(r, radius * scale.y);
		r = Max(r, radius * scale.z);
		radius = r;
	}

	float BoundingSphere::Distance(const BoundingSphere& sphere, const Vector3& point)
	{
		return sqrtf((point.x - sphere.center.x) * (point.x - sphere.center.x) +
					 (point.y - sphere.center.y) * (point.y - sphere.center.x) +
					 (point.z - sphere.center.z) * (point.z - sphere.center.x));
	}

	bool BoundingSphere::Contains(const BoundingSphere& sphere, const Vector3* points, unsigned int count)
	{
		for (unsigned int i = 0; i < count; i++)
		{
			if (Distance(sphere, points[i]) > sphere.radius)
			{
				return false;
			}
		}
		return true;
	}

} // namespace scythe