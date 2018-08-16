#ifndef __SCYTHE_SEGMENT_H__
#define __SCYTHE_SEGMENT_H__

#include "vector.h"

namespace scythe {

	/** Standard mathematical segment definition */
    struct Segment {
        Segment();
        Segment(const vec3& begin, const vec3& end);

        vec3 begin;
        vec3 end;
    };

} // namespace scythe

#endif