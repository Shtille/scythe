#ifndef __SCYTHE_TYPES_H__
#define __SCYTHE_TYPES_H__

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

#include <stddef.h>
#include <stdint.h>

#ifndef SCYTHE_PRECISION_DOUBLE
typedef float Float;
#else
typedef double Float;
#endif

// Signed types
typedef int8_t  S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
// Unsigned types
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
// Float types
typedef float  F32;
typedef double F64;

#endif