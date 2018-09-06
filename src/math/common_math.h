#ifndef __SCYTHE_COMMON_MATH_H__
#define __SCYTHE_COMMON_MATH_H__

#include "vector.h"
#include "matrix.h"

namespace scythe {
        
    // Math constants
    const float kPi     = 3.1415926535f;
    const float kTwoPi  = 6.2831853071f;
    
    // Sign function (returns -1 if negative, 1 otherwise)
    float Sign(float x);

    // Clamp function
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

	//! Vector orthogonalization via Gram-Shmidt method
	Vector3 Orthogonalize(const Vector3& v1, const Vector3& v2);

	// Projection matrices
	Matrix4 OrthoMatrix(float left, float right, float bottom, float top, float zNear, float zFar);
	Matrix4 PerspectiveMatrix(float fov, int width, int height, float zNear, float zFar); // NOTE: fov in degrees!
	// Camera matrices
	Matrix4 LookAt(const Vector3& from, const Vector3& at);
	Matrix4 LookAtCube(const Vector3& from, int face);
	Matrix4 ViewMatrix(const Vector3& dir, const Vector3& up, const Vector3& pos);
	Matrix4 ViewMatrix(const Matrix3& rot, const Vector3& pos);
	Matrix4 ViewHorizontalMirrorMatrix(const Matrix3& rot, const Vector3& pos, float h);
	// Object transformation matrices
	Matrix3 RotationMatrix(const Vector3& dir);
	Matrix4 RotationMatrix(const Vector3& dir, const Vector3& up);
	Matrix4 RotationMatrix(const Matrix3& m);
	Matrix4 OrientationMatrix(const Vector3& dir, const Vector3& up, const Vector3& pos);
	Matrix4 OrientationMatrix(const Matrix3& m, const Vector3& pos);
	// Other matrix generation functions
	Matrix3 Identity3(void);
	Matrix4 Identity4(void);
	Matrix4 TranslateMatrix(float x, float y, float z);
	Matrix4 TranslateMatrix(const Vector3& v);
	Matrix3 Scale3(float x);
	Matrix3 Scale3(float x, float y, float z);
	Matrix4 Scale4(float x);
	Matrix4 Scale4(float x, float y, float z);
	Matrix3 Rotate3(float c, float s, float x, float y, float z);
	Matrix4 Rotate4(float c, float s, float x, float y, float z);
    
    Matrix3 NormalMatrix(const Matrix4& modelview);
    
    float DistanceToCamera(const Vector3& world, const Matrix4& view);
    float DistanceToCamera(const Vector4& world, const Matrix4& view);

	void WorldToScreen(const Vector4& world, const Matrix4& proj, const Matrix4& view, const Vector4 viewport, Vector2 * screen);
	void ScreenToRay(const Vector2& screen, const Vector4& viewport, const Matrix4& proj, const Matrix4& view, Vector3 * ray);
    
    bool RaySphereIntersection(const Vector3& origin, const Vector3& direction, const Vector3& center, float radius, Vector3 * intersection);
    bool RayPlaneIntersection(const Vector3& origin, const Vector3& direction, const Vector4& plane, Vector3 * intersection);

} // namespace scythe

#endif