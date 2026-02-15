#include <scythe/math/bounding_box.h>

#include <scythe/math/ray.h>
#include <scythe/math/plane.h>
#include <scythe/math/frustum.h>
#include <scythe/math/bounding_sphere.h>
#include <scythe/math/constants.h>
#include <scythe/math/common.h>
#include <scythe/math/matrix4.h>
#include <scythe/defines.h>

namespace scythe {

	BoundingBox::BoundingBox()
	{
	}

	BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
	{
		Set(min, max);
	}

	BoundingBox::BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	{
		Set(minX, minY, minZ, maxX, maxY, maxZ);
	}

	BoundingBox::BoundingBox(const BoundingBox& copy)
	{
		Set(copy);
	}

	BoundingBox::~BoundingBox()
	{
	}

	const BoundingBox& BoundingBox::Empty()
	{
		static BoundingBox b;
		return b;
	}

	void BoundingBox::GetCorners(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

		// Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
		// Left-top-front.
		dst[0].Set(min.x, max.y, max.z);
		// Left-bottom-front.
		dst[1].Set(min.x, min.y, max.z);
		// Right-bottom-front.
		dst[2].Set(max.x, min.y, max.z);
		// Right-top-front.
		dst[3].Set(max.x, max.y, max.z);

		// Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
		// Right-top-back.
		dst[4].Set(max.x, max.y, min.z);
		// Right-bottom-back.
		dst[5].Set(max.x, min.y, min.z);
		// Left-bottom-back.
		dst[6].Set(min.x, min.y, min.z);
		// Left-top-back.
		dst[7].Set(min.x, max.y, min.z);
	}

	Vector3 BoundingBox::GetCenter() const
	{
		Vector3 center;
		GetCenter(&center);
		return center;
	}

	void BoundingBox::GetCenter(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

		dst->Set(min, max);
		dst->Scale(0.5f);
		dst->Add(min);
	}

	bool BoundingBox::Intersects(const BoundingSphere& sphere) const
	{
		return sphere.Intersects(*this);
	}

	bool BoundingBox::Intersects(const BoundingBox& box) const
	{
		return	((min.x >= box.min.x && min.x <= box.max.x) || (box.min.x >= min.x && box.min.x <= max.x)) &&
				((min.y >= box.min.y && min.y <= box.max.y) || (box.min.y >= min.y && box.min.y <= max.y)) &&
				((min.z >= box.min.z && min.z <= box.max.z) || (box.min.z >= min.z && box.min.z <= max.z));
	}

	bool BoundingBox::Intersects(const Frustum& frustum) const
	{
		// The box must either intersect or be in the positive half-space of all six planes of the frustum.
		return (Intersects(frustum.GetNear()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetFar()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetLeft()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetRight()) 	!= Plane::kIntersectionBack &&
				Intersects(frustum.GetBottom()) != Plane::kIntersectionBack &&
				Intersects(frustum.GetTop()) 	!= Plane::kIntersectionBack);
	}

	int BoundingBox::Intersects(const Plane& plane) const
	{
		// Calculate the distance from the center of the box to the plane.
		Vector3 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
		float distance = plane.Distance(center);

		// Get the extents of the box from its center along each axis.
		float extentX = (max.x - min.x) * 0.5f;
		float extentY = (max.y - min.y) * 0.5f;
		float extentZ = (max.z - min.z) * 0.5f;

		const Vector3& planeNormal = plane.GetNormal();
		if (Abs(distance) <= Abs(extentX * planeNormal.x) + Abs(extentY * planeNormal.y) + Abs(extentZ * planeNormal.z))
		{
			return Plane::kIntersectionExists;
		}

		return (distance > 0.0f) ? Plane::kIntersectionFront : Plane::kIntersectionBack;
	}

	bool BoundingBox::Intersects(const Ray& ray, float* distance) const
	{
		// Intermediate calculation variables.
		float dnear = 0.0f;
		float dfar = 0.0f;
		float tmin = 0.0f;
		float tmax = 0.0f;

		const Vector3& origin = ray.GetOrigin();
		const Vector3& direction = ray.GetDirection();

		// X direction.
		float div = 1.0f / direction.x;
		if (div >= 0.0f)
		{
			tmin = (min.x - origin.x) * div;
			tmax = (max.x - origin.x) * div;
		}
		else
		{
			tmin = (max.x - origin.x) * div;
			tmax = (min.x - origin.x) * div;
		}
		dnear = tmin;
		dfar = tmax;

		// Check if the ray misses the box.
		if (dnear > dfar || dfar < 0.0f)
		{
			return false;
		}

		// Y direction.
		div = 1.0f / direction.y;
		if (div >= 0.0f)
		{
			tmin = (min.y - origin.y) * div;
			tmax = (max.y - origin.y) * div;
		}
		else
		{
			tmin = (max.y - origin.y) * div;
			tmax = (min.y - origin.y) * div;
		}

		// Update the near and far intersection distances.
		if (tmin > dnear)
		{
			dnear = tmin;
		}
		if (tmax < dfar)
		{
			dfar = tmax;
		}
		// Check if the ray misses the box.
		if (dnear > dfar || dfar < 0.0f)
		{
			return false;
		}

		// Z direction.
		div = 1.0f / direction.z;
		if (div >= 0.0f)
		{
			tmin = (min.z - origin.z) * div;
			tmax = (max.z - origin.z) * div;
		}
		else
		{
			tmin = (max.z - origin.z) * div;
			tmax = (min.z - origin.z) * div;
		}

		// Update the near and far intersection distances.
		if (tmin > dnear)
		{
			dnear = tmin;
		}
		if (tmax < dfar)
		{
			dfar = tmax;
		}

		// Check if the ray misses the box.
		if (dnear > dfar || dfar < 0.0f)
		{
			return false;
		}
		// The ray intersects the box (and since the direction of a Ray is normalized, dnear is the distance to the ray).
		if (distance)
			*distance = dnear;
		return true;
	}

	bool BoundingBox::IsEmpty() const
	{
		return min.x == max.x && min.y == max.y && min.z == max.z;
	}

	void BoundingBox::Merge(const BoundingBox& box)
	{
		// Calculate the new minimum point.
		min.x = Min(min.x, box.min.x);
		min.y = Min(min.y, box.min.y);
		min.z = Min(min.z, box.min.z);

		// Calculate the new maximum point.
		max.x = Max(max.x, box.max.x);
		max.y = Max(max.y, box.max.y);
		max.z = Max(max.z, box.max.z);
	}

	void BoundingBox::Merge(const BoundingSphere& sphere)
	{
		const Vector3& center = sphere.center;
		float radius = sphere.radius;

		// Calculate the new minimum point for the merged bounding box.
		min.x = Min(min.x, center.x - radius);
		min.y = Min(min.y, center.y - radius);
		min.z = Min(min.z, center.z - radius);

		// Calculate the new maximum point for the merged bounding box.
		max.x = Max(max.x, center.x + radius);
		max.y = Max(max.y, center.y + radius);
		max.z = Max(max.z, center.z + radius);
	}

	void BoundingBox::Scale(float scale)
	{
		Vector3 center(0.5f * (max + min));
		Vector3 extent(0.5f * (max - min));
		extent *= scale;
		min = center - extent;
		max = center + extent;
	}

	void BoundingBox::Scale(const Vector3& scale)
	{
		Vector3 center(0.5f * (max + min));
		Vector3 extent(0.5f * (max - min));
		extent *= scale;
		min = center - extent;
		max = center + extent;
	}

	void BoundingBox::Set(const Vector3& min, const Vector3& max)
	{
		this->min = min;
		this->max = max;
	}

	void BoundingBox::Set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	{
		min.Set(minX, minY, minZ);
		max.Set(maxX, maxY, maxZ);
	}

	static void UpdateMinMax(Vector3* point, Vector3* min, Vector3* max)
	{
		SCYTHE_ASSERT(point);
		SCYTHE_ASSERT(min);
		SCYTHE_ASSERT(max);

		// Leftmost point.
		if (point->x < min->x)
		{
			min->x = point->x;
		}

		// Rightmost point.
		if (point->x > max->x)
		{
			max->x = point->x;
		}

		// Lowest point.
		if (point->y < min->y)
		{
			min->y = point->y;
		}

		// Highest point.
		if (point->y > max->y)
		{
			max->y = point->y;
		}

		// Farthest point.
		if (point->z < min->z)
		{
			min->z = point->z;
		}

		// Nearest point.
		if (point->z > max->z)
		{
			max->z = point->z;
		}
	}

	void BoundingBox::Set(const BoundingBox& box)
	{
		min = box.min;
		max = box.max;
	}

	void BoundingBox::Set(const BoundingSphere& sphere)
	{
		const Vector3& center = sphere.center;
		float radius = sphere.radius;

		// Calculate the minimum point for the box.
		min.x = center.x - radius;
		min.y = center.y - radius;
		min.z = center.z - radius;

		// Calculate the maximum point for the box.
		max.x = center.x + radius;
		max.y = center.y + radius;
		max.z = center.z + radius;
	}

	void BoundingBox::Transform(const Matrix4& matrix)
	{
		// Calculate the corners.
		Vector3 corners[8];
		GetCorners(corners);

		// Transform the corners, recalculating the min and max points along the way.
		matrix.TransformPoint(&corners[0]);
		Vector3 newMin = corners[0];
		Vector3 newMax = corners[0];
		for (int i = 1; i < 8; i++)
		{
			matrix.TransformPoint(&corners[i]);
			UpdateMinMax(&corners[i], &newMin, &newMax);
		}
		this->min.x = newMin.x;
		this->min.y = newMin.y;
		this->min.z = newMin.z;
		this->max.x = newMax.x;
		this->max.y = newMax.y;
		this->max.z = newMax.z;
	}

	void BoundingBox::Prepare()
	{
		Set(Vector3(kFloatMaximum), Vector3(kFloatMinimum));
	}

	void BoundingBox::AddPoint(const Vector3& point)
	{
		min.MakeMinimum(point);
		max.MakeMaximum(point);
	}

} // namespace scythe