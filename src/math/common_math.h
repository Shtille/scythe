#ifndef __SCYTHE_COMMON_MATH_H__
#define __SCYTHE_COMMON_MATH_H__

#include "vector2.h"
#include "matrix3.h"
#include "matrix4.h"
#include "constants.h"

namespace scythe {

	/**
	 * Standard sign function.
	 *
	 * @param[in] x The value.
	 * @return -1 if negative and 1 otherwise.
	 */
	float Sign(float x);

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
		if (x < a)
			return a;
		else if (b < x)
			return b;
		else
			return x;
	}

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

#endif