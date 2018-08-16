#ifndef __SCYTHE_GEOMETRY_LINE_H__
#define __SCYTHE_GEOMETRY_LINE_H__

#include "point.h"

namespace scythe {
namespace geometry {

	struct Line {
		Point origin;
		Vector direction;
	};
	
	struct Ray {
		Point origin;
		Vector direction;
	};

} // namespace geometry
} // namespace scythe

#endif