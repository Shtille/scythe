#ifndef __SCYTHE_MATH_LINE_H__
#define __SCYTHE_MATH_LINE_H__

#include "vector3.h"

namespace scythe {

	/** Standard mathematical line definition */
    struct Line {
        Vector3 origin;
		Vector3 direction;
    };

} // namespace scythe

#endif