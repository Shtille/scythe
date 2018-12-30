#ifndef __SCYTHE_MATH_RAY_H__
#define __SCYTHE_MATH_RAY_H__

#include "vector.h"

namespace scythe {

	/** Standard mathematical ray definition */
    struct Ray {
        vec3 origin;
        vec3 direction;
    };

} // namespace scythe

#endif