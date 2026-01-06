#ifndef __SCYTHE_DEFINES_H__
#define __SCYTHE_DEFINES_H__

// Attribute defines
#define SCYTHE_DEPRECATED [[deprecated]]
#define SCYTHE_MAYBE_UNUSED [[maybe_unused]]
#define SCYTHE_UNUSED(x) (void)(x)

// Delete defines
#define SCYTHE_SAFE_DELETE(x) if (x) { delete x; x = nullptr; }
#define SCYTHE_SAFE_DELETE_ARRAY(x) if (x) { delete[] x; x = nullptr; }
#define SCYTHE_SAFE_RELEASE(x) if (x) { x->Release(); x = nullptr; }

// Assert
#if defined(DEBUG) || defined(_DEBUG)
# include <cassert>
# define SCYTHE_ASSERT(x) assert(x)
#else
# define SCYTHE_ASSERT(x) (void)(x)
#endif

// Other
#define SCYTHE_MACRO_VARIABLE_NAME(x) (#x)
#define SCYTHE_MACRO_CONCAT_HELPER(A, B) A##B 		               // Helper macro to force expansion of arguments
#define SCYTHE_MACRO_CONCAT(A, B) SCYTHE_MACRO_CONCAT_HELPER(A, B) // Main concatenation macro

#endif