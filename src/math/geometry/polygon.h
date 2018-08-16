#ifndef __SCYTHE_GEOMETRY_POLYGON_H__
#define __SCYTHE_GEOMETRY_POLYGON_H__

#include "point.h"

#include <vector>

namespace scythe {
namespace geometry {

	struct Polygon {
		
		int Winding() const;
		void Append(const Point& point);
		void Swap(Polygon& other);

		/*! This works only if all of the following are true:
		 1. poly has no colinear edges;
		 2. poly has no duplicate vertices;
		 3. poly has at least three vertices;
		 4. poly is convex (implying 3).
		*/
		void Clip(const Polygon& frame, Polygon* out);

		std::vector<Point> points;
	};

} // namespace geometry
} // namespace scythe

#endif