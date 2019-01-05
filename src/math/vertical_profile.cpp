#include "vertical_profile.h"
#include "segment.h"
#include "vector2.h"

namespace scythe {

	VerticalProfile::VerticalProfile(const Vector3& a, const Vector3& b, float hmin, float hmax)
	: hmin(hmin)
	, hmax(hmax)
	{
		Vector2 ba(b.x - a.x, b.z - a.z);
		ba.Normalize();
		Vector3 normal(-ba.y, 0.0f, ba.x);
		float distance = -(normal & a);
		plane.Set(normal, distance);
	}
	Vector3 VerticalProfile::GetAnyPoint()
	{
		Vector3 p;
		p.y = hmin;
		if (plane.GetNormal().x != 0.0f) // use x
		{
			p.x = 0.0f;
			p.z = -plane.GetDistance() / plane.GetNormal().z;
		}
		else // use z
		{
			p.z = 0.0f;
			p.x = -plane.GetDistance() / plane.GetNormal().x;
		}
		return p;
	}
	bool VerticalProfile::InRange(const Segment& segment) const
	{
		return (segment.begin.y > hmin && segment.begin.y < hmax) ||
			   (segment.end.y   > hmin && segment.end.y   < hmax) ||
			   (segment.begin.y < hmin && segment.end.y   > hmax) ||
			   (segment.end.y   < hmin && segment.begin.y > hmax);
	}

} // namespace scythe