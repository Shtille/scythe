#ifndef __SCYTHE_VERTICAL_PROFILE_H__
#define __SCYTHE_VERTICAL_PROFILE_H__

#include "plane.h"

namespace scythe {
	
	// Forward declarations
	struct Segment;

	/** Vertical profile definition */
	struct VerticalProfile {
		VerticalProfile(const vec3& a, const vec3& b, float hmin, float hmax);

		vec3 GetAnyPoint(); //!< returns any point that lies in the profile
		bool InRange(const Segment& segment) const; //!< is segment in height range

		Plane plane;	//!< vertical plane
		float hmin;		//!< minimum height of the plane
		float hmax;		//!< maximum height of the plane
	};

} // namespace scythe

#endif