#include "segment.h"

namespace scythe {

	Segment::Segment()
	{
	}
	Segment::Segment(const vec3& begin, const vec3& end)
	: begin(begin)
	, end(end)
	{
	}

} // namespace scythe