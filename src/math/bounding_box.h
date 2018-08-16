#ifndef __SCYTHE_BOUNDING_BOX_H__
#define __SCYTHE_BOUNDING_BOX_H__

#include "vector.h"

namespace scythe {

	/** Bounding box definition */
    struct BoundingBox {
        vec3 center;
        vec3 extent;
    };

} // namespace scythe

#endif