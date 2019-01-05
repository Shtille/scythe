#ifndef __SCYTHE_SEGMENT_H__
#define __SCYTHE_SEGMENT_H__

#include "vector3.h"

namespace scythe {

	/** Standard mathematical segment definition */
    struct Segment {
        Segment();
        Segment(const Vector3& begin, const Vector3& end);

		Vector3 begin;
		Vector3 end;
    };

} // namespace scythe

#endif