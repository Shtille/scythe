#include "segment.h"

namespace scythe {

	Segment::Segment()
	{
	}
	Segment::Segment(const Vector3& begin, const Vector3& end)
	: begin(begin)
	, end(end)
	{
	}

} // namespace scythe