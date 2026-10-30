#ifndef __SCYTHE_COLOR_H__
#define __SCYTHE_COLOR_H__

#include "types.h"

namespace scythe {

    struct Color
    {
        Color();

        F32 red;
        F32 green;
        F32 blue;
        F32 alpha;
    };

} // namespace scythe

#endif