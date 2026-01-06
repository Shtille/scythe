#ifndef __SCYTHE_TYPES_H__
#define __SCYTHE_TYPES_H__

#include <stddef.h>
#include <stdint.h>

#ifndef _countof
# define _countof(x) sizeof(x)/sizeof(x[0])
#endif

namespace scythe {

// Signed types
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;

// Unsigned types
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;

// Float types
typedef float F32;
typedef double F64;

} // namespace scythe

#endif