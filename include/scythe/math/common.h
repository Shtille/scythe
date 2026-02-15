#ifndef __SCYTHE_MATH_COMMON_H__
#define __SCYTHE_MATH_COMMON_H__

#ifndef SCYTHE_USE_MATH
# error "Math should be enabled to use this header"
#endif

#include <limits>
#include <type_traits>

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix4.h"

namespace scythe {

	/**
	 * @brief      Requires template type to be arithmetic.
	 */
	template <typename T>
	inline void RequireArithmetic()
	{
		static_assert(std::is_arithmetic<T>::value, "Type should be arithmetic");
	}

	/**
	 * @brief      Requires template type to be floating point.
	 */
	template <typename T>
	inline void RequireFloatingPoint()
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point");
	}

	/**
	 * @brief      Requires template type to be signed.
	 */
	template <typename T>
	inline void RequireSigned()
	{
		static_assert(std::is_signed<T>::value, "Type should be signed");
	}

	/**
	 * Standard sign function.
	 *
	 * @param[in] x The value.
	 * @return -1 if negative and 1 otherwise.
	 */
	template <typename T>
	inline T Sign(T x)
	{
		RequireSigned<T>();
		constexpr T kZero = static_cast<T>(0);
		constexpr T kMinusOne = static_cast<T>(-1);
		constexpr T kPlusOne = static_cast<T>(+1);
		return (x < kZero) ? kMinusOne : kPlusOne;
	}

	/**
	 * @brief      Calculates an absolute value.
	 *
	 * @param[in]  x     The value
	 * @return     The absolute value.
	 */
	template <typename T>
	inline T Abs(T x)
	{
		RequireArithmetic<T>();
		constexpr T kZero = static_cast<T>(0);
		return (x < kZero) ? (-x) : (x);
	}

	/**
	 * Standard min function.
	 *
	 * @param[in] a The first value.
	 * @param[in] b The second value.
	 * @return Minimum value of two.
	 */
	template <typename T>
	inline T Min(T a, T b)
	{
		RequireArithmetic<T>();
		return (a < b) ? (a) : (b);
	}

	/**
	 * Standard max function.
	 *
	 * @param[in] a The first value.
	 * @param[in] b The second value.
	 * @return Maximum value of two.
	 */
	template <typename T>
	inline T Max(T a, T b)
	{
		RequireArithmetic<T>();
		return (a < b) ? (b) : (a);
	}

	/**
	 * Standard clamp function.
	 *
	 * @param[in] x The value.
	 * @param[in] a The lower bound.
	 * @param[in] b The upper bound.
	 * @return Value clamped to range [a; b].
	 */
	template <typename T>
	inline T Clamp(T x, T a, T b)
	{
		RequireArithmetic<T>();
		if (x < a)
			return a;
		else if (b < x)
			return b;
		else
			return x;
	}

	/**
	 * @brief      Standard square function.
	 *
	 * @param[in]  x     The value.
	 * @return     The square of the value.
	 */
	template <typename T>
	constexpr T Sqr(T x)
	{
		RequireArithmetic<T>();
		return x * x;
	}

	/**
	 * @brief      Checks if floating point values are equal.
	 *
	 * @param[in]  a     The first value.
	 * @param[in]  b     The second value.
	 * @return     True if equal and false otherwise.
	 */
	template <typename T>
	bool AreEqual(T a, T b)
	{
		RequireFloatingPoint<T>();
		T diff = Abs(a - b);
		a = Abs(a);
		b = Abs(b);
		// Use the larger of the two magnitudes to scale the epsilon
		T largest = Max(a, b);
		constexpr T max_relative_diff = std::numeric_limits<T>::epsilon();
		return diff <= largest * max_relative_diff;
	}

	/**
	 * @brief      Finds the closest point on line.
	 * @details    The result is following:
	 *             * if point projection lies before the first point, the first point is returned;
	 *             * if point projection lies after the second point, the second point is returned;
	 *             * otherwise point projection is returned.
	 *
	 * @param[in]  a     The first point of line.
	 * @param[in]  b     The second point of line.
	 * @param[in]  p     The point to be projected.
	 *
	 * @return     The closest point on line.
	 */
	Vector3 ClosestPointOnLine(const Vector3& a, const Vector3& b, const Vector3& p);

	/**
	 * Vector orthogonalization via Gram-Shmidt method
	 *
	 * @todo Add proper documentation for this function.
	 */
	Vector3 Orthogonalize(const Vector3& v1, const Vector3& v2);
	
	/**
	 * Calculates distance to camera.
	 *
	 * @param[in] world The 3-component world position.
	 * @param[in] view The view matrix.
	 * @return Distance to camera.
	 */
	float DistanceToCamera(const Vector3& world, const Matrix4& view);

	/**
	 * Calculates distance to camera.
	 *
	 * @param[in] world The 4-component world position.
	 * @param[in] view The view matrix.
	 * @return Distance to camera.
	 */
	float DistanceToCamera(const Vector4& world, const Matrix4& view);

	/**
	 * Converts world coordinates to screen.
	 *
	 * @param[in] world The 4-component world position vector.
	 * @param[in] proj The projection matrix.
	 * @param[in] view The view matrix.
	 * @param[in] viewport The viewport vector.
	 * @param[out] screen The screen coordinates vector.
	 */
	void WorldToScreen(const Vector4& world, const Matrix4& proj, const Matrix4& view, const Vector4& viewport, Vector2 * screen);

	/**
	 * Converts screen coordinates to world ray.
	 *
	 * @param[in] screen The screen coordinates vector.
	 * @param[in] viewport The viewport vector.
	 * @param[in] proj The projection matrix.
	 * @param[in] view The view matrix.
	 * @param[out] ray The ray in world coordinates system.
	 */
	void ScreenToRay(const Vector2& screen, const Vector4& viewport, const Matrix4& proj, const Matrix4& view, Vector3 * ray);
	
	/**
	 * Computes intersection between ray and sphere.
	 *
	 * @param[in] origin The ray origin vector.
	 * @param[in] direction The ray direction vector.
	 * @param[in] center The sphere center vector.
	 * @param[in] radius The sphere radius.
	 * @param[opt,out] intersection The intersection result.
	 * @return True if the ray intersects the sphere and false otherwise.
	 */
	bool RaySphereIntersection(const Vector3& origin, const Vector3& direction, const Vector3& center, float radius, Vector3 * intersection);

	/**
	 * Computes intersection between ray and plane.
	 *
	 * @param[in] origin The ray origin vector.
	 * @param[in] direction The ray direction vector.
	 * @param[in] plane The plane equation vector.
	 * @param[opt,out] intersection The intersection result.
	 * @return True if the ray intersects the plane and false otherwise.
	 */
	bool RayPlaneIntersection(const Vector3& origin, const Vector3& direction, const Vector4& plane, Vector3 * intersection);

} // namespace scythe

#endif // __SCYTHE_MATH_COMMON_H__