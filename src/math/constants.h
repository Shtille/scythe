#ifndef __SCYTHE_MATH_CONSTANTS_H__
#define __SCYTHE_MATH_CONSTANTS_H__

#define MATH_DEG_TO_RAD(x)			((x) * 0.0174532925f)
#define MATH_RAD_TO_DEG(x)			((x) * 57.29577951f)

namespace scythe {

	// Common constants
	static const float kE 		= 2.71828182845904523536f;
	static const float kLog10E 	= 0.4342944819032518f;
	static const float kLog2E 	= 1.442695040888963387f;
	static const float kPi 		= 3.14159265358979323846f;
    static const float kHalfPi 	= 1.57079632679489661923f;
    static const float kTwoPi 	= 6.28318530717958647693f;

	// Small value
	static const float kFloatSmall = 1e-37f;

	// Tolerance
	static const float kFloatTolerance = 2e-37f;

	// Epsilon
	static const float kFloatEpsilon = 1e-7f;
	static const double kDoubleEpsilon = 1e-15;

} // namespace scythe

#endif