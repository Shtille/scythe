#include <scythe/math/matrix4.h>

#include <cmath>
#include <cstring>

#include <scythe/math/matrix3.h>
#include <scythe/math/quaternion.h>
#include <scythe/math/plane.h>
#include <scythe/math/constants.h>
#include <scythe/defines.h>
#include <scythe/log.h>

namespace scythe {

	static const size_t kMatrixSize = (sizeof(float) * 16);
	static const float kMatrixIdentity[16] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4::Matrix4()
	{
		*this = Matrix4::Identity();
	}

	Matrix4::Matrix4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
				   float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
	{
		Set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
	}

	Matrix4::Matrix4(const float* m)
	{
		Set(m);
	}

	Matrix4::Matrix4(const Matrix4& copy)
	{
		memcpy(m, copy.m, kMatrixSize);
	}

	Matrix4::~Matrix4()
	{
	}

	const Matrix4& Matrix4::Identity()
	{
		static Matrix4 m(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 );
		return m;
	}

	const Matrix4& Matrix4::Zero()
	{
		static Matrix4 m(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0 );
		return m;
	}

	void Matrix4::CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up, Matrix4* dst)
	{
		CreateLookAt(eye.x, eye.y, eye.z, target.x, target.y, target.z,
					 up.x, up.y, up.z, dst);
	}

	void Matrix4::CreateLookAt(float eye_x, float eye_y, float eye_z,
							  float target_x, float target_y, float target_z,
							  float up_x, float up_y, float up_z, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		Vector3 eye(eye_x, eye_y, eye_z);
		Vector3 target(target_x, target_y, target_z);
		Vector3 up(up_x, up_y, up_z);
		up.Normalize();

		Vector3 zaxis;
		Vector3::Subtract(eye, target, &zaxis);
		zaxis.Normalize();

		Vector3 xaxis;
		Vector3::Cross(up, zaxis, &xaxis);
		xaxis.Normalize();

		Vector3 yaxis;
		Vector3::Cross(zaxis, xaxis, &yaxis);
		yaxis.Normalize();

		dst->m[0] = xaxis.x;
		dst->m[1] = yaxis.x;
		dst->m[2] = zaxis.x;
		dst->m[3] = 0.0f;

		dst->m[4] = xaxis.y;
		dst->m[5] = yaxis.y;
		dst->m[6] = zaxis.y;
		dst->m[7] = 0.0f;

		dst->m[8] = xaxis.z;
		dst->m[9] = yaxis.z;
		dst->m[10] = zaxis.z;
		dst->m[11] = 0.0f;

		dst->m[12] = -Vector3::Dot(xaxis, eye);
		dst->m[13] = -Vector3::Dot(yaxis, eye);
		dst->m[14] = -Vector3::Dot(zaxis, eye);
		dst->m[15] = 1.0f;
	}

	void Matrix4::CreateLookAtCube(const Vector3& eye, int face, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		switch (face)
		{
		case 0: // +X
			dst->Set(
				0.0f, 0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 1: // -X
			dst->Set(
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 2: // +Y
			dst->Set(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 3: // -Y
			dst->Set(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, -1.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 4: // +Z
			dst->Set(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, -1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 5: // -Z
			dst->Set(
				-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			break;
		}
		dst->Translate(-eye);
	}

	void Matrix4::CreateView(const Vector3& forward, const Vector3& up, const Vector3& side,
							 const Vector3& eye, Matrix4* dst)
	{
		//      |	s[0]   s[1]   s[2]  -s.e  |
		//      |	u[0]   u[1]   u[2]  -u.e  |
		//  V = |  -f[0]  -f[1]  -f[2]   f.e  |
		//      |	 0	    0      0      1   |

		SCYTHE_ASSERT(dst);

		dst->m[0] = side.x;
		dst->m[1] = up.x;
		dst->m[2] = -forward.x;
		dst->m[3] = 0.0f;

		dst->m[4] = side.y;
		dst->m[5] = up.y;
		dst->m[6] = -forward.y;
		dst->m[7] = 0.0f;

		dst->m[8] = side.z;
		dst->m[9] = up.z;
		dst->m[10] = -forward.z;
		dst->m[11] = 0.0f;

		dst->m[12] = -Vector3::Dot(side, eye);
		dst->m[13] = -Vector3::Dot(up, eye);
		dst->m[14] = Vector3::Dot(forward, eye);
		dst->m[15] = 1.0f;
	}

	void Matrix4::CreateView(const Matrix3& rotation, const Vector3& eye, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		Vector3 forward, up, side;
		rotation.GetForwardVector(&forward);
		rotation.GetUpVector(&up);
		rotation.GetRightVector(&side);

		CreateView(forward, up, side, eye, dst);
	}

	void Matrix4::CreateView(const Quaternion& q, const Vector3& eye, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		Matrix3 rotation;
		Matrix3::CreateRotation(q, &rotation);

		CreateView(rotation, eye, dst);
	}

	void Matrix4::CreatePerspective(float fieldOfView, float aspectRatio,
								   float zNearPlane, float zFarPlane, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);
		SCYTHE_ASSERT(zFarPlane != zNearPlane);

		float f_n = 1.0f / (zFarPlane - zNearPlane);
		float theta = fieldOfView * kDegreesToRadians * 0.5f;
		if (fabs(fmod(theta, kHalfPi)) < kFloatEpsilon)
		{
			Error("Invalid field of view value (%d) causes attempted calculation tan(%d), which is undefined.", fieldOfView, theta);
			return;
		}
		float divisor = tan(theta);
		SCYTHE_ASSERT(divisor);
		float factor = 1.0f / divisor;

		memset(dst, 0, kMatrixSize);

		SCYTHE_ASSERT(aspectRatio);
		dst->m[0] = (1.0f / aspectRatio) * factor;
		dst->m[5] = factor;
		dst->m[10] = (-(zFarPlane + zNearPlane)) * f_n;
		dst->m[11] = -1.0f;
		dst->m[14] = -2.0f * zFarPlane * zNearPlane * f_n;
	}

	void Matrix4::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane, Matrix4* dst)
	{
		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;
		CreateOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, dst);
	}

	void Matrix4::CreateOrthographicOffCenter(float left, float right, float bottom, float top,
											 float zNearPlane, float zFarPlane, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);
		SCYTHE_ASSERT(right != left);
		SCYTHE_ASSERT(top != bottom);
		SCYTHE_ASSERT(zFarPlane != zNearPlane);

		memset(dst, 0, kMatrixSize);
		dst->m[0] = 2.0f / (right - left);
		dst->m[5] = 2.0f / (top - bottom);
		dst->m[12] = (left + right) / (left - right);
		dst->m[10] = 1.0f / (zNearPlane - zFarPlane);
		dst->m[13] = (top + bottom) / (bottom - top);
		dst->m[14] = zNearPlane / (zNearPlane - zFarPlane);
		dst->m[15] = 1.0f;
	}
		
	void Matrix4::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition,
								 const Vector3& cameraUpVector, Matrix4* dst)
	{
		CreateBillboardHelper(objectPosition, cameraPosition, cameraUpVector, nullptr, dst);
	}

	void Matrix4::CreateBillboard(const Vector3& objectPosition, const Vector3& cameraPosition,
								 const Vector3& cameraUpVector, const Vector3& cameraForwardVector,
								 Matrix4* dst)
	{
		CreateBillboardHelper(objectPosition, cameraPosition, cameraUpVector, &cameraForwardVector, dst);
	}

	void Matrix4::CreateBillboardHelper(const Vector3& objectPosition, const Vector3& cameraPosition,
									   const Vector3& cameraUpVector, const Vector3* cameraForwardVector,
									   Matrix4* dst)
	{
		Vector3 delta(objectPosition, cameraPosition);
		bool isSufficientDelta = delta.LengthSquared() > kFloatEpsilon;

		dst->SetIdentity();
		dst->m[3] = objectPosition.x;
		dst->m[7] = objectPosition.y;
		dst->m[11] = objectPosition.z;

		// As per the contracts for the 2 variants of createBillboard, we need
		// either a safe default or a sufficient distance between object and camera.
		if (cameraForwardVector || isSufficientDelta)
		{
			Vector3 target = isSufficientDelta ? cameraPosition : (objectPosition - *cameraForwardVector);

			// A billboard is the inverse of a lookAt rotation
			Matrix4 lookAt;
			CreateLookAt(objectPosition, target, cameraUpVector, &lookAt);
			dst->m[0] = lookAt.m[0];
			dst->m[1] = lookAt.m[4];
			dst->m[2] = lookAt.m[8];
			dst->m[4] = lookAt.m[1];
			dst->m[5] = lookAt.m[5];
			dst->m[6] = lookAt.m[9];
			dst->m[8] = lookAt.m[2];
			dst->m[9] = lookAt.m[6];
			dst->m[10] = lookAt.m[10];
		}
	}
		
	void Matrix4::CreateReflection(const Plane& plane, Matrix4* dst)
	{
		Vector3 normal(plane.GetNormal());
		float k = -2.0f * plane.GetDistance();

		dst->SetIdentity();

		dst->m[0] -= 2.0f * normal.x * normal.x;
		dst->m[5] -= 2.0f * normal.y * normal.y;
		dst->m[10] -= 2.0f * normal.z * normal.z;
		dst->m[1] = dst->m[4] = -2.0f * normal.x * normal.y;
		dst->m[2] = dst->m[8] = -2.0f * normal.x * normal.z;
		dst->m[6] = dst->m[9] = -2.0f * normal.y * normal.z;
		
		dst->m[3] = k * normal.x;
		dst->m[7] = k * normal.y;
		dst->m[11] = k * normal.z;
	}

	void Matrix4::CreateScale(const Vector3& scale, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		dst->m[0] = scale.x;
		dst->m[5] = scale.y;
		dst->m[10] = scale.z;
	}

	void Matrix4::CreateScale(float xScale, float yScale, float zScale, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		dst->m[0] = xScale;
		dst->m[5] = yScale;
		dst->m[10] = zScale;
	}


	void Matrix4::CreateRotation(const Quaternion& q, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		float x2 = q.x + q.x;
		float y2 = q.y + q.y;
		float z2 = q.z + q.z;

		float xx2 = q.x * x2;
		float yy2 = q.y * y2;
		float zz2 = q.z * z2;
		float xy2 = q.x * y2;
		float xz2 = q.x * z2;
		float yz2 = q.y * z2;
		float wx2 = q.w * x2;
		float wy2 = q.w * y2;
		float wz2 = q.w * z2;

		dst->m[0] = 1.0f - yy2 - zz2;
		dst->m[1] = xy2 + wz2;
		dst->m[2] = xz2 - wy2;
		dst->m[3] = 0.0f;

		dst->m[4] = xy2 - wz2;
		dst->m[5] = 1.0f - xx2 - zz2;
		dst->m[6] = yz2 + wx2;
		dst->m[7] = 0.0f;

		dst->m[8] = xz2 + wy2;
		dst->m[9] = yz2 - wx2;
		dst->m[10] = 1.0f - xx2 - yy2;
		dst->m[11] = 0.0f;

		dst->m[12] = 0.0f;
		dst->m[13] = 0.0f;
		dst->m[14] = 0.0f;
		dst->m[15] = 1.0f;
	}

	void Matrix4::CreateRotation(const Vector3& axis, float angle, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		float x = axis.x;
		float y = axis.y;
		float z = axis.z;

		// Make sure the input axis is normalized.
		float n = x*x + y*y + z*z;
		if (n != 1.0f)
		{
			// Not normalized.
			n = sqrt(n);
			// Prevent divide too close to zero.
			if (n > kFloatTolerance)
			{
				n = 1.0f / n;
				x *= n;
				y *= n;
				z *= n;
			}
		}

		float c = cos(angle);
		float s = sin(angle);

		float t = 1.0f - c;
		float tx = t * x;
		float ty = t * y;
		float tz = t * z;
		float txy = tx * y;
		float txz = tx * z;
		float tyz = ty * z;
		float sx = s * x;
		float sy = s * y;
		float sz = s * z;

		dst->m[0] = c + tx*x;
		dst->m[1] = txy + sz;
		dst->m[2] = txz - sy;
		dst->m[3] = 0.0f;

		dst->m[4] = txy - sz;
		dst->m[5] = c + ty*y;
		dst->m[6] = tyz + sx;
		dst->m[7] = 0.0f;

		dst->m[8] = txz + sy;
		dst->m[9] = tyz - sx;
		dst->m[10] = c + tz*z;
		dst->m[11] = 0.0f;

		dst->m[12] = 0.0f;
		dst->m[13] = 0.0f;
		dst->m[14] = 0.0f;
		dst->m[15] = 1.0f;
	}

	void Matrix4::CreateRotation(const Vector3& forward, const Vector3& up,
							     const Vector3& side, Matrix4* dst)
	{
		//      |	f[0]   u[0]   s[0]  0  |
		//      |	f[1]   u[1]   s[1]  0  |
		//  M = |   f[2]   u[2]   s[2]  0  |
		//      |	 0	    0      0    1  |

		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		dst->m[0] = side.x;
		dst->m[1] = side.y;
		dst->m[2] = side.z;
#else
		dst->m[0] = forward.x;
		dst->m[1] = forward.y;
		dst->m[2] = forward.z;
#endif
		dst->m[3] = 0.0f;

		dst->m[4] = up.x;
		dst->m[5] = up.y;
		dst->m[6] = up.z;
		dst->m[7] = 0.0f;

#ifdef SCYTHE_ORIENTATION_Z
		dst->m[8] = -forward.x;
		dst->m[9] = -forward.y;
		dst->m[10] = -forward.z;
#else
		dst->m[8] = side.x;
		dst->m[9] = side.y;
		dst->m[10] = side.z;
#endif
		dst->m[11] = 0.0f;

		dst->m[12] = 0.0f;
		dst->m[13] = 0.0f;
		dst->m[14] = 0.0f;
		dst->m[15] = 1.0f;
	}

	void Matrix4::CreateRotationX(float angle, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		float c = cos(angle);
		float s = sin(angle);

		dst->m[5]  = c;
		dst->m[6]  = s;
		dst->m[9]  = -s;
		dst->m[10] = c;
	}

	void Matrix4::CreateRotationY(float angle, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		float c = cos(angle);
		float s = sin(angle);

		dst->m[0]  = c;
		dst->m[2]  = -s;
		dst->m[8]  = s;
		dst->m[10] = c;
	}

	void Matrix4::CreateRotationZ(float angle, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		float c = cos(angle);
		float s = sin(angle);

		dst->m[0] = c;
		dst->m[1] = s;
		dst->m[4] = -s;
		dst->m[5] = c;
	}

	void Matrix4::CreateFromEuler(float yaw, float pitch, float roll, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);
		
		dst->RotateY(yaw);
		dst->RotateX(pitch);
		dst->RotateZ(roll);
	}

	void Matrix4::CreateTranslation(const Vector3& translation, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		dst->m[12] = translation.x;
		dst->m[13] = translation.y;
		dst->m[14] = translation.z;
	}

	void Matrix4::CreateTranslation(float xTranslation, float yTranslation, float zTranslation, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		dst->m[12] = xTranslation;
		dst->m[13] = yTranslation;
		dst->m[14] = zTranslation;
	}

	void Matrix4::CreateOrientation(const Vector3& forward, const Vector3& up,
									const Vector3& side, const Vector3& pos, Matrix4* dst)
	{
		//      |	f[0]   u[0]   s[0]  p[0]  |
		//      |	f[1]   u[1]   s[1]  p[1]  |
		//  M = |   f[2]   u[2]   s[2]  p[2]  |
		//      |	 0	    0      0     1    |

		SCYTHE_ASSERT(dst);

		dst->m[0] = forward.x;
		dst->m[1] = forward.y;
		dst->m[2] = forward.z;
		dst->m[3] = 0.0f;

		dst->m[4] = up.x;
		dst->m[5] = up.y;
		dst->m[6] = up.z;
		dst->m[7] = 0.0f;

		dst->m[8] = side.x;
		dst->m[9] = side.y;
		dst->m[10] = side.z;
		dst->m[11] = 0.0f;

		dst->m[12] = pos.x;
		dst->m[13] = pos.y;
		dst->m[14] = pos.z;
		dst->m[15] = 1.0f;
	}

	void Matrix4::Add(float scalar)
	{
		Add(scalar, this);
	}

	void Matrix4::Add(float scalar, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		dst->m[0]  = m[0]  + scalar;
		dst->m[1]  = m[1]  + scalar;
		dst->m[2]  = m[2]  + scalar;
		dst->m[3]  = m[3]  + scalar;
		dst->m[4]  = m[4]  + scalar;
		dst->m[5]  = m[5]  + scalar;
		dst->m[6]  = m[6]  + scalar;
		dst->m[7]  = m[7]  + scalar;
		dst->m[8]  = m[8]  + scalar;
		dst->m[9]  = m[9]  + scalar;
		dst->m[10] = m[10] + scalar;
		dst->m[11] = m[11] + scalar;
		dst->m[12] = m[12] + scalar;
		dst->m[13] = m[13] + scalar;
		dst->m[14] = m[14] + scalar;
		dst->m[15] = m[15] + scalar;
	}

	void Matrix4::Add(const Matrix4& m)
	{
		Add(*this, m, this);
	}

	void Matrix4::Add(const Matrix4& m1, const Matrix4& m2, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		dst->m[0]  = m1.m[0]  + m2.m[0];
		dst->m[1]  = m1.m[1]  + m2.m[1];
		dst->m[2]  = m1.m[2]  + m2.m[2];
		dst->m[3]  = m1.m[3]  + m2.m[3];
		dst->m[4]  = m1.m[4]  + m2.m[4];
		dst->m[5]  = m1.m[5]  + m2.m[5];
		dst->m[6]  = m1.m[6]  + m2.m[6];
		dst->m[7]  = m1.m[7]  + m2.m[7];
		dst->m[8]  = m1.m[8]  + m2.m[8];
		dst->m[9]  = m1.m[9]  + m2.m[9];
		dst->m[10] = m1.m[10] + m2.m[10];
		dst->m[11] = m1.m[11] + m2.m[11];
		dst->m[12] = m1.m[12] + m2.m[12];
		dst->m[13] = m1.m[13] + m2.m[13];
		dst->m[14] = m1.m[14] + m2.m[14];
		dst->m[15] = m1.m[15] + m2.m[15];
	}

	bool Matrix4::Decompose(Vector3* scale, Quaternion* rotation, Vector3* translation) const
	{
		if (translation)
		{
			// Extract the translation.
			translation->x = m[12];
			translation->y = m[13];
			translation->z = m[14];
		}

		// Nothing left to do.
		if (scale == nullptr && rotation == nullptr)
			return true;

		// Extract the scale.
		// This is simply the length of each axis (row/column) in the matrix.
		Vector3 xaxis(m[0], m[1], m[2]);
		float scaleX = xaxis.Length();

		Vector3 yaxis(m[4], m[5], m[6]);
		float scaleY = yaxis.Length();

		Vector3 zaxis(m[8], m[9], m[10]);
		float scaleZ = zaxis.Length();

		// Determine if we have a negative scale (true if determinant is less than zero).
		// In this case, we simply negate a single axis of the scale.
		float det = Determinant();
		if (det < 0)
			scaleZ = -scaleZ;

		if (scale)
		{
			scale->x = scaleX;
			scale->y = scaleY;
			scale->z = scaleZ;
		}

		// Nothing left to do.
		if (rotation == nullptr)
			return true;

		// Scale too close to zero, can't decompose rotation.
		if (scaleX < kFloatTolerance || scaleY < kFloatTolerance || fabs(scaleZ) < kFloatTolerance)
			return false;

		float rn;

		// Factor the scale out of the matrix axes.
		rn = 1.0f / scaleX;
		xaxis.x *= rn;
		xaxis.y *= rn;
		xaxis.z *= rn;

		rn = 1.0f / scaleY;
		yaxis.x *= rn;
		yaxis.y *= rn;
		yaxis.z *= rn;

		rn = 1.0f / scaleZ;
		zaxis.x *= rn;
		zaxis.y *= rn;
		zaxis.z *= rn;

		// Now calculate the rotation from the resulting matrix (axes).
		float trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

		if (trace > 1.0f)
		{
			float s = 0.5f / sqrt(trace);
			rotation->w = 0.25f / s;
			rotation->x = (yaxis.z - zaxis.y) * s;
			rotation->y = (zaxis.x - xaxis.z) * s;
			rotation->z = (xaxis.y - yaxis.x) * s;
		}
		else
		{
			// Note: since xaxis, yaxis, and zaxis are normalized, 
			// we will never divide by zero in the code below.
			if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
			{
				float s = 0.5f / sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
				rotation->w = (yaxis.z - zaxis.y) * s;
				rotation->x = 0.25f / s;
				rotation->y = (yaxis.x + xaxis.y) * s;
				rotation->z = (zaxis.x + xaxis.z) * s;
			}
			else if (yaxis.y > zaxis.z)
			{
				float s = 0.5f / sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
				rotation->w = (zaxis.x - xaxis.z) * s;
				rotation->x = (yaxis.x + xaxis.y) * s;
				rotation->y = 0.25f / s;
				rotation->z = (zaxis.y + yaxis.z) * s;
			}
			else
			{
				float s = 0.5f / sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y );
				rotation->w = (xaxis.y - yaxis.x ) * s;
				rotation->x = (zaxis.x + xaxis.z ) * s;
				rotation->y = (zaxis.y + yaxis.z ) * s;
				rotation->z = 0.25f / s;
			}
		}

		return true;
	}

	float Matrix4::Determinant() const
	{
		float a0 = m[0] * m[5] - m[1] * m[4];
		float a1 = m[0] * m[6] - m[2] * m[4];
		float a2 = m[0] * m[7] - m[3] * m[4];
		float a3 = m[1] * m[6] - m[2] * m[5];
		float a4 = m[1] * m[7] - m[3] * m[5];
		float a5 = m[2] * m[7] - m[3] * m[6];
		float b0 = m[8] * m[13] - m[9] * m[12];
		float b1 = m[8] * m[14] - m[10] * m[12];
		float b2 = m[8] * m[15] - m[11] * m[12];
		float b3 = m[9] * m[14] - m[10] * m[13];
		float b4 = m[9] * m[15] - m[11] * m[13];
		float b5 = m[10] * m[15] - m[11] * m[14];

		// Calculate the determinant.
		return (a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0);
	}

	void Matrix4::GetScale(Vector3* scale) const
	{
		Decompose(scale, nullptr, nullptr);
	}

	bool Matrix4::GetRotation(Quaternion* rotation) const
	{
		return Decompose(nullptr, rotation, nullptr);
	}

	void Matrix4::GetTranslation(Vector3* translation) const
	{
		Decompose(nullptr, nullptr, translation);
	}

	void Matrix4::GetUpVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

		// Use +Y as up
		dst->x = m[4];
		dst->y = m[5];
		dst->z = m[6];
	}

	void Matrix4::GetDownVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);
		
		// Use -Y as down
		dst->x = -m[4];
		dst->y = -m[5];
		dst->z = -m[6];
	}

	void Matrix4::GetLeftVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use -X as left
		dst->x = -m[0];
		dst->y = -m[1];
		dst->z = -m[2];
#else
		// Use -Z as left
		dst->x = -m[8];
		dst->y = -m[9];
		dst->z = -m[10];
#endif
	}

	void Matrix4::GetRightVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use +X as right
		dst->x = m[0];
		dst->y = m[1];
		dst->z = m[2];
#else
		// Use +Z as right
		dst->x = m[8];
		dst->y = m[9];
		dst->z = m[10];
#endif
	}

	void Matrix4::GetForwardVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use -Z as forward
		dst->x = -m[8];
		dst->y = -m[9];
		dst->z = -m[10];
#else
		// Use +X as forward
		dst->x = m[0];
		dst->y = m[1];
		dst->z = m[2];
#endif
	}

	void Matrix4::GetBackVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use +Z as backward
		dst->x = m[8];
		dst->y = m[9];
		dst->z = m[10];
#else
		// Use -X as backward
		dst->x = -m[0];
		dst->y = -m[1];
		dst->z = -m[2];
#endif
	}

	bool Matrix4::Invert()
	{
		return Invert(this);
	}

	bool Matrix4::Invert(Matrix4* dst) const
	{
		float a0 = m[0] * m[5] - m[1] * m[4];
		float a1 = m[0] * m[6] - m[2] * m[4];
		float a2 = m[0] * m[7] - m[3] * m[4];
		float a3 = m[1] * m[6] - m[2] * m[5];
		float a4 = m[1] * m[7] - m[3] * m[5];
		float a5 = m[2] * m[7] - m[3] * m[6];
		float b0 = m[8] * m[13] - m[9] * m[12];
		float b1 = m[8] * m[14] - m[10] * m[12];
		float b2 = m[8] * m[15] - m[11] * m[12];
		float b3 = m[9] * m[14] - m[10] * m[13];
		float b4 = m[9] * m[15] - m[11] * m[13];
		float b5 = m[10] * m[15] - m[11] * m[14];

		// Calculate the determinant.
		float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

		// Close to zero, can't invert.
		if (fabs(det) <= kFloatTolerance)
			return false;

		// Support the case where m == dst.
		Matrix4 inverse;
		inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
		inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
		inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
		inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;

		inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
		inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
		inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
		inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;

		inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
		inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
		inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
		inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

		inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
		inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
		inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
		inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

		Multiply(inverse, 1.0f / det, dst);

		return true;
	}

	bool Matrix4::IsIdentity() const
	{
		return (memcmp(m, kMatrixIdentity, kMatrixSize) == 0);
	}

	void Matrix4::Multiply(float scalar)
	{
		Multiply(scalar, this);
	}

	void Matrix4::Multiply(float scalar, Matrix4* dst) const
	{
		Multiply(*this, scalar, dst);
	}

	void Matrix4::Multiply(const Matrix4& m, float scalar, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		dst->m[0]  = m.m[0]  * scalar;
		dst->m[1]  = m.m[1]  * scalar;
		dst->m[2]  = m.m[2]  * scalar;
		dst->m[3]  = m.m[3]  * scalar;
		dst->m[4]  = m.m[4]  * scalar;
		dst->m[5]  = m.m[5]  * scalar;
		dst->m[6]  = m.m[6]  * scalar;
		dst->m[7]  = m.m[7]  * scalar;
		dst->m[8]  = m.m[8]  * scalar;
		dst->m[9]  = m.m[9]  * scalar;
		dst->m[10] = m.m[10] * scalar;
		dst->m[11] = m.m[11] * scalar;
		dst->m[12] = m.m[12] * scalar;
		dst->m[13] = m.m[13] * scalar;
		dst->m[14] = m.m[14] * scalar;
		dst->m[15] = m.m[15] * scalar;
	}

	void Matrix4::Multiply(const Matrix4& m)
	{
		Multiply(*this, m, this);
	}

	void Matrix4::Multiply(const Matrix4& m1, const Matrix4& m2, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		// Support the case where m1 or m2 is the same array as dst.
		float product[16];

		product[0]  = m1.m[0] * m2.m[0]  + m1.m[4] * m2.m[1] + m1.m[8]   * m2.m[2]  + m1.m[12] * m2.m[3];
		product[1]  = m1.m[1] * m2.m[0]  + m1.m[5] * m2.m[1] + m1.m[9]   * m2.m[2]  + m1.m[13] * m2.m[3];
		product[2]  = m1.m[2] * m2.m[0]  + m1.m[6] * m2.m[1] + m1.m[10]  * m2.m[2]  + m1.m[14] * m2.m[3];
		product[3]  = m1.m[3] * m2.m[0]  + m1.m[7] * m2.m[1] + m1.m[11]  * m2.m[2]  + m1.m[15] * m2.m[3];

		product[4]  = m1.m[0] * m2.m[4]  + m1.m[4] * m2.m[5] + m1.m[8]   * m2.m[6]  + m1.m[12] * m2.m[7];
		product[5]  = m1.m[1] * m2.m[4]  + m1.m[5] * m2.m[5] + m1.m[9]   * m2.m[6]  + m1.m[13] * m2.m[7];
		product[6]  = m1.m[2] * m2.m[4]  + m1.m[6] * m2.m[5] + m1.m[10]  * m2.m[6]  + m1.m[14] * m2.m[7];
		product[7]  = m1.m[3] * m2.m[4]  + m1.m[7] * m2.m[5] + m1.m[11]  * m2.m[6]  + m1.m[15] * m2.m[7];

		product[8]  = m1.m[0] * m2.m[8]  + m1.m[4] * m2.m[9] + m1.m[8]   * m2.m[10] + m1.m[12] * m2.m[11];
		product[9]  = m1.m[1] * m2.m[8]  + m1.m[5] * m2.m[9] + m1.m[9]   * m2.m[10] + m1.m[13] * m2.m[11];
		product[10] = m1.m[2] * m2.m[8]  + m1.m[6] * m2.m[9] + m1.m[10]  * m2.m[10] + m1.m[14] * m2.m[11];
		product[11] = m1.m[3] * m2.m[8]  + m1.m[7] * m2.m[9] + m1.m[11]  * m2.m[10] + m1.m[15] * m2.m[11];

		product[12] = m1.m[0] * m2.m[12] + m1.m[4] * m2.m[13] + m1.m[8]  * m2.m[14] + m1.m[12] * m2.m[15];
		product[13] = m1.m[1] * m2.m[12] + m1.m[5] * m2.m[13] + m1.m[9]  * m2.m[14] + m1.m[13] * m2.m[15];
		product[14] = m1.m[2] * m2.m[12] + m1.m[6] * m2.m[13] + m1.m[10] * m2.m[14] + m1.m[14] * m2.m[15];
		product[15] = m1.m[3] * m2.m[12] + m1.m[7] * m2.m[13] + m1.m[11] * m2.m[14] + m1.m[15] * m2.m[15];

		memcpy(dst->m, product, kMatrixSize);
	}

	void Matrix4::Negate()
	{
		Negate(this);
	}

	void Matrix4::Negate(Matrix4* dst) const
	{
		SCYTHE_ASSERT(dst);

		dst->m[0]  = -m[0];
		dst->m[1]  = -m[1];
		dst->m[2]  = -m[2];
		dst->m[3]  = -m[3];
		dst->m[4]  = -m[4];
		dst->m[5]  = -m[5];
		dst->m[6]  = -m[6];
		dst->m[7]  = -m[7];
		dst->m[8]  = -m[8];
		dst->m[9]  = -m[9];
		dst->m[10] = -m[10];
		dst->m[11] = -m[11];
		dst->m[12] = -m[12];
		dst->m[13] = -m[13];
		dst->m[14] = -m[14];
		dst->m[15] = -m[15];
	}

	void Matrix4::Rotate(const Quaternion& q)
	{
		Rotate(q, this);
	}

	void Matrix4::Rotate(const Quaternion& q, Matrix4* dst) const
	{
		Matrix4 r;
		CreateRotation(q, &r);
		Multiply(*this, r, dst);
	}

	void Matrix4::Rotate(const Vector3& axis, float angle)
	{
		Rotate(axis, angle, this);
	}

	void Matrix4::Rotate(const Vector3& axis, float angle, Matrix4* dst) const
	{
		Matrix4 r;
		CreateRotation(axis, angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix4::RotateX(float angle)
	{
		RotateX(angle, this);
	}

	void Matrix4::RotateX(float angle, Matrix4* dst) const
	{
		Matrix4 r;
		CreateRotationX(angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix4::RotateY(float angle)
	{
		RotateY(angle, this);
	}

	void Matrix4::RotateY(float angle, Matrix4* dst) const
	{
		Matrix4 r;
		CreateRotationY(angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix4::RotateZ(float angle)
	{
		RotateZ(angle, this);
	}

	void Matrix4::RotateZ(float angle, Matrix4* dst) const
	{
		Matrix4 r;
		CreateRotationZ(angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix4::Scale(float value)
	{
		Scale(value, this);
	}

	void Matrix4::Scale(float value, Matrix4* dst) const
	{
		Scale(value, value, value, dst);
	}

	void Matrix4::Scale(float xScale, float yScale, float zScale)
	{
		Scale(xScale, yScale, zScale, this);
	}

	void Matrix4::Scale(float xScale, float yScale, float zScale, Matrix4* dst) const
	{
		Matrix4 s;
		CreateScale(xScale, yScale, zScale, &s);
		Multiply(*this, s, dst);
	}

	void Matrix4::Scale(const Vector3& s)
	{
		Scale(s.x, s.y, s.z, this);
	}

	void Matrix4::Scale(const Vector3& s, Matrix4* dst) const
	{
		Scale(s.x, s.y, s.z, dst);
	}

	void Matrix4::Set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
					 float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
	{
		m[0]  = m11;
		m[1]  = m21;
		m[2]  = m31;
		m[3]  = m41;
		m[4]  = m12;
		m[5]  = m22;
		m[6]  = m32;
		m[7]  = m42;
		m[8]  = m13;
		m[9]  = m23;
		m[10] = m33;
		m[11] = m43;
		m[12] = m14;
		m[13] = m24;
		m[14] = m34;
		m[15] = m44;
	}

	void Matrix4::Set(const float* m)
	{
		SCYTHE_ASSERT(m);
		memcpy(this->m, m, kMatrixSize);
	}

	void Matrix4::Set(const Matrix4& m)
	{
		memcpy(this->m, m.m, kMatrixSize);
	}

	void Matrix4::SetIdentity()
	{
		memcpy(m, kMatrixIdentity, kMatrixSize);
	}

	void Matrix4::SetZero()
	{
		memset(m, 0, kMatrixSize);
	}

	void Matrix4::Subtract(const Matrix4& m)
	{
		Subtract(*this, m, this);
	}

	void Matrix4::Subtract(const Matrix4& m1, const Matrix4& m2, Matrix4* dst)
	{
		SCYTHE_ASSERT(dst);

		dst->m[0]  = m1.m[0]  - m2.m[0];
		dst->m[1]  = m1.m[1]  - m2.m[1];
		dst->m[2]  = m1.m[2]  - m2.m[2];
		dst->m[3]  = m1.m[3]  - m2.m[3];
		dst->m[4]  = m1.m[4]  - m2.m[4];
		dst->m[5]  = m1.m[5]  - m2.m[5];
		dst->m[6]  = m1.m[6]  - m2.m[6];
		dst->m[7]  = m1.m[7]  - m2.m[7];
		dst->m[8]  = m1.m[8]  - m2.m[8];
		dst->m[9]  = m1.m[9]  - m2.m[9];
		dst->m[10] = m1.m[10] - m2.m[10];
		dst->m[11] = m1.m[11] - m2.m[11];
		dst->m[12] = m1.m[12] - m2.m[12];
		dst->m[13] = m1.m[13] - m2.m[13];
		dst->m[14] = m1.m[14] - m2.m[14];
		dst->m[15] = m1.m[15] - m2.m[15];
	}

	void Matrix4::TransformPoint(Vector3* point) const
	{
		SCYTHE_ASSERT(point);
		TransformVector(point->x, point->y, point->z, 1.0f, point);
	}

	void Matrix4::TransformPoint(const Vector3& point, Vector3* dst) const
	{
		TransformVector(point.x, point.y, point.z, 1.0f, dst);
	}

	void Matrix4::TransformVector(Vector3* vector) const
	{
		SCYTHE_ASSERT(vector);
		TransformVector(vector->x, vector->y, vector->z, 0.0f, vector);
	}

	void Matrix4::TransformVector(const Vector3& vector, Vector3* dst) const
	{
		TransformVector(vector.x, vector.y, vector.z, 0.0f, dst);
	}

	void Matrix4::TransformVector(float x, float y, float z, float w, Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

		dst->x = x * m[0] + y * m[4] + z * m[8] + w * m[12];
		dst->y = x * m[1] + y * m[5] + z * m[9] + w * m[13];
		dst->z = x * m[2] + y * m[6] + z * m[10] + w * m[14];
	}

	void Matrix4::TransformVector(Vector4* vector) const
	{
		SCYTHE_ASSERT(vector);
		TransformVector(*vector, vector);
	}

	void Matrix4::TransformVector(const Vector4& vector, Vector4* dst) const
	{
		SCYTHE_ASSERT(dst);

		const float* v = &vector.x;
		// Handle case where v == dst.
		float x = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12];
		float y = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13];
		float z = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14];
		float w = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15];

		dst->x = x;
		dst->y = y;
		dst->z = z;
		dst->w = w;
	}

	void Matrix4::Translate(float x, float y, float z)
	{
		Translate(x, y, z, this);
	}

	void Matrix4::Translate(float x, float y, float z, Matrix4* dst) const
	{
		Matrix4 t;
		CreateTranslation(x, y, z, &t);
		Multiply(*this, t, dst);
	}

	void Matrix4::Translate(const Vector3& t)
	{
		Translate(t.x, t.y, t.z, this);
	}

	void Matrix4::Translate(const Vector3& t, Matrix4* dst) const
	{
		Translate(t.x, t.y, t.z, dst);
	}

	void Matrix4::Transpose()
	{
		Transpose(this);
	}

	void Matrix4::Transpose(Matrix4* dst) const
	{
		SCYTHE_ASSERT(dst);

		float t[16] = {
			m[0], m[4], m[8], m[12],
			m[1], m[5], m[9], m[13],
			m[2], m[6], m[10], m[14],
			m[3], m[7], m[11], m[15]
		};
		memcpy(dst->m, t, kMatrixSize);
	}

} // namespace scythe