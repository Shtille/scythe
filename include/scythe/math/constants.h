#ifndef __SCYTHE_MATH_CONSTANTS_H__
#define __SCYTHE_MATH_CONSTANTS_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include <limits>

namespace scythe {

	// Angles
	static const float kDegreesToRadians 	= 0.01745329251994329577f;
	static const float kRadiansToDegrees 	= 57.2957795130823208768f;

	// Common constants
	static const float kE 					= 2.71828182845904523536f;
	static const float kLog10E 				= 0.43429448190325182765f;
	static const float kLog2E 				= 1.44269504088896340736f;
	static const float kPi 					= 3.14159265358979323846f;
	static const float kHalfPi 				= 1.57079632679489661923f;
	static const float kTwoPi 				= 6.28318530717958647693f;

	// Small value
	static const float kFloatSmall = std::numeric_limits<float>::min();

	// Minimum value
	static const float kFloatMinimum = std::numeric_limits<float>::lowest();

	// Maximum value
	static const float kFloatMaximum = std::numeric_limits<float>::max(); // 3.4028235e+38

	// Tolerance
	static const float kFloatTolerance = 2e-37f; // 1.0f / kFloatMaximum

	// Epsilon
	static const float kFloatEpsilon = std::numeric_limits<float>::epsilon();
	static const double kDoubleEpsilon = std::numeric_limits<double>::epsilon();

	// Infinity
	static const float kFloatInfinity = std::numeric_limits<float>::infinity();
	static const float kDoubleInfinity = std::numeric_limits<double>::infinity();

} // namespace scythe

#endif // __SCYTHE_MATH_CONSTANTS_H__