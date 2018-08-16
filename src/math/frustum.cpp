#include "frustum.h"

#include "line.h"
#include "segment.h"
#include "bounding_box.h"
#include "vertical_profile.h"

#include <cmath>

namespace scythe {

	int Frustum::opposite_planes_[6] = {1,0,3,2,5,4}; // opposite planes' indices

	const vec3& Frustum::GetDir() const
	{
		return planes_[kPlaneNear].normal;
	}
	void Frustum::Load(const float *mvp)
	{        
		// mvp is column-major matrix
		planes_[kPlaneLeft  ] = Plane(mvp[3] + mvp[0], mvp[7] + mvp[4], mvp[11] + mvp[8],  mvp[15] + mvp[12]);
		planes_[kPlaneRight ] = Plane(mvp[3] - mvp[0], mvp[7] - mvp[4], mvp[11] - mvp[8],  mvp[15] - mvp[12]);

		planes_[kPlaneTop   ] = Plane(mvp[3] - mvp[1], mvp[7] - mvp[5], mvp[11] - mvp[9],  mvp[15] - mvp[13]);
		planes_[kPlaneBottom] = Plane(mvp[3] + mvp[1], mvp[7] + mvp[5], mvp[11] + mvp[9],  mvp[15] + mvp[13]);

		planes_[kPlaneFar   ] = Plane(mvp[3] - mvp[2], mvp[7] - mvp[6], mvp[11] - mvp[10], mvp[15] - mvp[14]);
		planes_[kPlaneNear  ] = Plane(mvp[3] + mvp[2], mvp[7] + mvp[6], mvp[11] + mvp[10], mvp[15] + mvp[14]);
	}
	void Frustum::LoadTransposed(const float *mvp)
	{
		planes_[kPlaneLeft  ] = Plane(mvp[12] + mvp[0], mvp[13] + mvp[1], mvp[14] + mvp[2],  mvp[15] + mvp[3]);
		planes_[kPlaneRight ] = Plane(mvp[12] - mvp[0], mvp[13] - mvp[1], mvp[14] - mvp[2],  mvp[15] - mvp[3]);

		planes_[kPlaneTop   ] = Plane(mvp[12] - mvp[4], mvp[13] - mvp[5], mvp[14] - mvp[6],  mvp[15] - mvp[7]);
		planes_[kPlaneBottom] = Plane(mvp[12] + mvp[4], mvp[13] + mvp[5], mvp[14] + mvp[6],  mvp[15] + mvp[7]);

		planes_[kPlaneFar   ] = Plane(mvp[12] - mvp[8], mvp[13] - mvp[9], mvp[14] - mvp[10], mvp[15] - mvp[11]);
		planes_[kPlaneNear  ] = Plane(mvp[12] + mvp[8], mvp[13] + mvp[9], mvp[14] + mvp[10], mvp[15] + mvp[11]);
	}
	bool Frustum::IsPointIn(const vec3& p) const
	{
		for (int i = 0; i < 6; ++i)
		if ((p & planes_[i].normal) + planes_[i].offset <= 0.0f)
			return false;
		return true;
	}
	bool Frustum::IsSphereIn(const vec3& p, float r) const
	{
		for (int i = 0; i < 6; ++i)
		if ((p & planes_[i].normal) + planes_[i].offset <= -r)
			return false;
		return true;
	}
	bool Frustum::IsPolygonIn(int num_points, vec3 *p) const
	{
		for (int i = 0; i < 6; ++i)
		{
			bool skip = false;
			for (int j = 0; j < num_points; ++j)
			if (planes_[i].normal.x*p[j].x +
				planes_[i].normal.y*p[j].y +
				planes_[i].normal.z*p[j].z +
				planes_[i].offset >= 0.0f)
			{
				skip = true;
				break;
			}
			if (skip)
				continue;

			return false;
		}
		return true;
	}
	bool Frustum::IsSegmentIn(const Segment& segment) const
	{
		for (int i = 0; i < 6; ++i)
		{
			if (planes_[i].Distance(segment.begin) >= 0.0f ||
				planes_[i].Distance(segment.end) >= 0.0f)
				continue;

			return false;
		}
		return true;
	}
	bool Frustum::IsBoxIn(const vec2& p, float min_h, float max_h, float size) const
	{
		for (int i = 0; i < 6; ++i)
		{
			if (planes_[i].normal.x*(p.x - size) +
				planes_[i].normal.y*min_h +
				planes_[i].normal.z*(p.y - size) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + size) +
				planes_[i].normal.y*min_h +
				planes_[i].normal.z*(p.y - size) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + size) +
				planes_[i].normal.y*min_h +
				planes_[i].normal.z*(p.y + size) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x - size) +
				planes_[i].normal.y*min_h +
				planes_[i].normal.z*(p.y + size) +
				planes_[i].offset >= 0.0f)
				continue;

			if (planes_[i].normal.x*(p.x - size) +
				planes_[i].normal.y*max_h +
				planes_[i].normal.z*(p.y - size) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + size) +
				planes_[i].normal.y*max_h +
				planes_[i].normal.z*(p.y - size) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + size) +
				planes_[i].normal.y*max_h +
				planes_[i].normal.z*(p.y + size) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x - size) +
				planes_[i].normal.y*max_h +
				planes_[i].normal.z*(p.y + size) +
				planes_[i].offset >= 0.0f)
				continue;

			return false;
		}
		return true;
	}
	bool Frustum::IsBoxIn(const vec3& p, float sx, float sy, float sz) const
	{
		for (int i = 0; i < 6; ++i)
		{
			if (planes_[i].normal.x*(p.x - sx) +
				planes_[i].normal.y*(p.y - sy) +
				planes_[i].normal.z*(p.z - sz) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + sx) +
				planes_[i].normal.y*(p.y - sy) +
				planes_[i].normal.z*(p.z - sz) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + sx) +
				planes_[i].normal.y*(p.y - sy) +
				planes_[i].normal.z*(p.z + sz) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x - sx) +
				planes_[i].normal.y*(p.y - sy) +
				planes_[i].normal.z*(p.z + sz) +
				planes_[i].offset >= 0.0f)
				continue;

			if (planes_[i].normal.x*(p.x - sx) +
				planes_[i].normal.y*(p.y + sy) +
				planes_[i].normal.z*(p.z - sz) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + sx) +
				planes_[i].normal.y*(p.y + sy) +
				planes_[i].normal.z*(p.z - sz) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x + sx) +
				planes_[i].normal.y*(p.y + sy) +
				planes_[i].normal.z*(p.z + sz) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(p.x - sx) +
				planes_[i].normal.y*(p.y + sy) +
				planes_[i].normal.z*(p.z + sz) +
				planes_[i].offset >= 0.0f)
				continue;

			return false;
		}
		return true;
	}
	bool Frustum::IsBoxIn(const BoundingBox& bb) const
	{
		for (int i = 0; i < 6; ++i)
		{
			if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
				planes_[i].normal.y*(bb.center.y - bb.extent.y) +
				planes_[i].normal.z*(bb.center.z - bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
				planes_[i].normal.y*(bb.center.y - bb.extent.y) +
				planes_[i].normal.z*(bb.center.z - bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
				planes_[i].normal.y*(bb.center.y - bb.extent.y) +
				planes_[i].normal.z*(bb.center.z + bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
				planes_[i].normal.y*(bb.center.y - bb.extent.y) +
				planes_[i].normal.z*(bb.center.z + bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;

			if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
				planes_[i].normal.y*(bb.center.y + bb.extent.y) +
				planes_[i].normal.z*(bb.center.z - bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
				planes_[i].normal.y*(bb.center.y + bb.extent.y) +
				planes_[i].normal.z*(bb.center.z - bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(bb.center.x + bb.extent.x) +
				planes_[i].normal.y*(bb.center.y + bb.extent.y) +
				planes_[i].normal.z*(bb.center.z + bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*(bb.center.x - bb.extent.x) +
				planes_[i].normal.y*(bb.center.y + bb.extent.y) +
				planes_[i].normal.z*(bb.center.z + bb.extent.z) +
				planes_[i].offset >= 0.0f)
				continue;

			return false;
		}
		return true;
	}
	bool Frustum::IsPlaneIn(float sx, float sz, float ex, float ez, float h) const
	{
		for (int i = 0; i < 6; ++i) // enum planes
		{
			if (planes_[i].normal.x*sx +
				planes_[i].normal.y*h  +
				planes_[i].normal.z*sz +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*ex +
				planes_[i].normal.y*h  +
				planes_[i].normal.z*sz +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*sx +
				planes_[i].normal.y*h  +
				planes_[i].normal.z*ez +
				planes_[i].offset >= 0.0f)
				continue;
			if (planes_[i].normal.x*ex +
				planes_[i].normal.y*h  +
				planes_[i].normal.z*ez +
				planes_[i].offset >= 0.0f)
				continue;

			return false;
		}
		return true;
	}
	CullInfo Frustum::ComputeBoxVisibility(const vec3& center, const vec3& extent, CullInfo in) const
	{
		// Check the box against each active frustum plane.
		int    bit = 1;
		for (int i = 0; i < 6; i++, bit <<= 1)
		{
			if ((bit & in.active_planes) == 0)
			{
				// This plane is already known to not cull the box.
				continue;
			}

			// Check box against this plane.
			float d = (planes_[i].normal & center) + planes_[i].offset; // was - p.d
			float extent_toward_plane = std::abs(extent.x * planes_[i].normal.x)
									  + std::abs(extent.y * planes_[i].normal.y)
									  + std::abs(extent.z * planes_[i].normal.z);
			if (d < 0)
			{
				if (-d > extent_toward_plane)
				{
					// Box is culled by plane; it's not visible.
					return CullInfo(true, 0);
				} // else this plane is ambiguous so leave it active.
			}
			else
			{
				if (d > extent_toward_plane)
				{
					// Box is accepted by this plane, so
					// deactivate it, since neither this
					// box or any contained part of it can
					// ever be culled by this plane.
					in.active_planes &= ~bit;
					if (in.active_planes == 0)
					{
						// This box is definitively inside all the culling
						// planes_, so there's no need to continue.
						return in;
					}
				} // else this plane is ambigious so leave it active.
			}
		}

		return in;    // Box not definitively culled.  Return updated active plane flags.
	}
	int Frustum::IntersectionsWithSegment(const Segment& segment, vec3 points[2]) const
	{
		int num_intersections = 0;
		vec3 p;

		for (int i = 0; i < 6; ++i)
		{
			if (!planes_[i].IntersectsSegment(segment.begin, segment.end, p))
				continue;

			bool good = true;
			for (int j = 0; j < 6; ++j)
			{
				if (j == i || j == opposite_planes_[i]) // dont need to test this and opposite planes
					continue;

				// Distance to the each plane should be positive
				good = (planes_[j].normal & p) + planes_[j].offset >= 0.0f;
				if (!good)
					break;
			}
			if (!good)
				continue;

			// We've found one good intersection
			points[num_intersections] = p;
			++num_intersections;

			if (num_intersections == 2)
				break;
		}
		return num_intersections;
	}
	int Frustum::IntersectionsWithPlane(const Plane& plane, Segment segments[6]) const
	{
		int num_segments = 0;
		for (int i = 0; i < 6; ++i)
		{
			Line line;
			if (!planes_[i].IntersectsPlane(plane, line))
				continue;

			if (!CropLine(line, i, segments[num_segments]))
				continue;

			// 3. Add cropped segment to the list
			++num_segments;
		}
		return num_segments;
	}
	int Frustum::IntersectionsWithProfile(const VerticalProfile& profile, Segment segments[6]) const
	{
		Plane lower_bound(0.0f, 1.0f, 0.0f, -profile.hmin);
		Plane upper_bound(0.0f, -1.0f, 0.0f, profile.hmax);

		int num_segments = 0;
		for (int i = 0; i < 6; ++i)
		{
			Line line;
			if (!planes_[i].IntersectsPlane(profile.plane, line))
				continue;

			if (!CropLine(line, i, segments[num_segments]))
				continue;

			// Test whether segment lies within profile
			if (!profile.InRange(segments[num_segments]))
				continue;

			// Also we have to crop segment by vertical profile's bounds
			lower_bound.CropSegment(segments[num_segments].begin, segments[num_segments].end);
			upper_bound.CropSegment(segments[num_segments].begin, segments[num_segments].end);

			// 3. Add cropped segment to the list
			++num_segments;
		}
		return num_segments;
	}
	bool Frustum::CropLine(const Line& line, int index, Segment& cropped_segment) const
	{
		vec3 points[2];
		int num_intersections = 0;
		for (int i = 0; i < 6; ++i)
		{
			// Line should be cropped by the 4 others planes
			if (i == index || i == opposite_planes_[index]) // we do only need to test 4 planes
				continue;

			// We will try to find two intersections
			vec3 & p = points[num_intersections];
			if (!planes_[i].IntersectsLine(line, p))
				continue;

			// Found one, test is it in frustum
			bool good = true;
			for (int j = 0; j < 6; ++j)
			{
				if (j == index || j == opposite_planes_[index] ||
					j == i || j == opposite_planes_[i]) // dont need to test this and opposite planes
					continue;

				// Distance to the each plane should be positive
				good = planes_[j].Distance(p) >= 0.0f;
				if (!good)
					break;
			}
			if (!good)
				continue;

			// We've found one good intersection
			++num_intersections;

			// We're done
			if (num_intersections == 2)
			{
				// Order of points doesn't matter
				cropped_segment.begin = points[0];
				cropped_segment.end = points[1];
				return true;
			}
		}
		return false;
	}

} // namespace scythe
