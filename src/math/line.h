#ifndef __SCYTHE_MATH_LINE_H__
#define __SCYTHE_MATH_LINE_H__

#include "vector.h"

namespace scythe {

	/** Standard mathematical line definition */
    struct Line {
        vec3 origin;
        vec3 direction;
    };

} // namespace scythe

#endif