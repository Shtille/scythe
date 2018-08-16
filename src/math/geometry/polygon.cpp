#include "polygon.h"
#include "line.h"

#include <stdlib.h>
#include <cmath>

namespace sht {
namespace math {
namespace geometry {

	// Precision (all values below this will be threated as zero)
	const float kEpsilon = 0.0001f;

	static inline float cross(const Point& a, const Point& b)
	{
		return a.x * b.y - a.y * b.x;
	}
	/* tells if vec c lies on the left side of directed edge a->b
	 * 1 if left, -1 if right, 0 if colinear
	 */
	static int left_of(const Point& a, const Point& b, const Point& c)
	{
		Vector ba = b - a;
		Vector cb = c - b;
		float x = cross(ba, cb);
		return x < -kEpsilon ? -1 : x > kEpsilon;
	}
	static bool line_sect(const Point& x0, const Point& x1, 
		const Point& y0, const Point& y1, Point * res)
	{
	    Vector dx, dy, d;
	    dx = x1 - x0;
	    dy = y1 - y0;
	    d = x0 - y0;
	    /* x0 + a dx = y0 + b dy ->
	       x0 X dx = y0 X dx + b dy X dx ->
	       b = (x0 - y0) X dx / (dy X dx) */
	    float dyx = cross(dy, dx);
	    if (fabs(dyx) < kEpsilon)
	    	return false;
	    dyx = cross(d, dx) / dyx;
	    if (dyx <= 0.0f || dyx >= 1.0f)
	    	return false;
	 
	    res->x = y0.x + dyx * dy.x;
	    res->y = y0.y + dyx * dy.y;
	    return true;
	}
	int Polygon::Winding() const
	{
		return left_of(points[0], points[1], points[2]);
	}
	void Polygon::Append(const Point& point)
	{
		points.push_back(point);
	}
	void Polygon::Swap(Polygon& other)
	{
		points.swap(other.points);
	}
	static inline void EdgeClip(const Polygon * poly, const Point& x0, const Point& x1, int left, Polygon * res)
	{
		Vector tmp;
		Point v0 = poly->points.back();
		res->points.clear();
 
		int side0 = left_of(x0, x1, v0);
		if (side0 != -left)
			res->Append(v0);
 
		for (size_t i = 0; i < poly->points.size(); ++i)
		{
			const Point& v1 = poly->points[i];
			int side1 = left_of(x0, x1, v1);
			if (side0 + side1 == 0 && side0)
				/* last point and current straddle the edge */
				if (line_sect(x0, x1, v0, v1, &tmp))
					res->Append(tmp);
			if (i == poly->points.size() - 1)
				break;
			if (side1 != -left)
				res->Append(v1);
			v0 = v1;
			side0 = side1;
		}
	}
	void Polygon::Clip(const Polygon& frame, Polygon* out)
	{
		Polygon temp;
		int dir = frame.Winding();
		EdgeClip(this, frame.points.back(), frame.points.front(), dir, out);
		for (size_t i = 0; (frame.points.size()) && (i < frame.points.size() - 1); ++i)
		{
			out->Swap(temp);
			if (temp.points.empty())
			{
				out->points.clear();
				break;
			}
			EdgeClip(&temp, frame.points[i], frame.points[i+1], dir, out);
		}
	}

} // namespace geometry
} // namespace math
} // namespace sht