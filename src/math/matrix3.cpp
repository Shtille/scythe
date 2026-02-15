#include <scythe/math/matrix3.h>

#include <cmath>
#include <cstring>

#include <scythe/math/matrix4.h>
#include <scythe/math/quaternion.h>
#include <scythe/math/constants.h>
#include <scythe/defines.h>

namespace scythe {

	static const size_t kMatrixSize = (sizeof(float) * 9);
	static const float kMatrixIdentity[9] =
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	Matrix3::Matrix3()
	{
		*this = Matrix3::Identity();
	}

	Matrix3::Matrix3(float m11, float m12, float m13, float m21, float m22, float m23,
		   float m31, float m32, float m33)
	{
		Set(m11, m12, m13, m21, m22, m23, m31, m32, m33);
	}

	Matrix3::Matrix3(const float* m)
	{
		Set(m);
	}

	Matrix3::Matrix3(const Matrix3& copy)
	{
		memcpy(m, copy.m, kMatrixSize);
	}

	Matrix3::~Matrix3()
	{
	}

	const Matrix3& Matrix3::Identity()
	{
		static Matrix3 m(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1 );
		return m;
	}

	const Matrix3& Matrix3::Zero()
	{
		static Matrix3 m(
			0, 0, 0,
			0, 0, 0,
			0, 0, 0 );
		return m;
	}

	void Matrix3::CreateBasis(const Vector3& x_axis, const Vector3& y_axis, Matrix3* dst)
	{
		//      |	f[0]   u[0]   s[0]   |
		//  M = |	f[1]   u[1]   s[1]   |
		//      |   f[2]   u[2]   s[2]   |
		SCYTHE_ASSERT(dst);
		Vector3 forward, up, side;
		x_axis.Normalize(&forward);
		Vector3::Cross(x_axis, y_axis, &side);
		side.Normalize();
		Vector3::Cross(side, forward, &up);
		up.Normalize();

#ifdef SCYTHE_ORIENTATION_Z
		dst->m[0] = side.x;
		dst->m[1] = side.y;
		dst->m[2] = side.z;
#else
		dst->m[0] = forward.x;
		dst->m[1] = forward.y;
		dst->m[2] = forward.z;
#endif

		dst->m[3] = up.x;
		dst->m[4] = up.y;
		dst->m[5] = up.z;

#ifdef SCYTHE_ORIENTATION_Z
		dst->m[6] = -forward.x;
		dst->m[7] = -forward.y;
		dst->m[8] = -forward.z;
#else
		dst->m[6] = side.x;
		dst->m[7] = side.y;
		dst->m[8] = side.z;
#endif
	}

	void Matrix3::CreateRotation(const Quaternion& q, Matrix3* dst)
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

		dst->m[3] = xy2 - wz2;
		dst->m[4] = 1.0f - xx2 - zz2;
		dst->m[5] = yz2 + wx2;

		dst->m[6] = xz2 + wy2;
		dst->m[7] = yz2 - wx2;
		dst->m[8] = 1.0f - xx2 - yy2;
	}

	void Matrix3::CreateRotation(const Vector3& axis, float angle, Matrix3* dst)
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

		dst->m[3] = txy - sz;
		dst->m[4] = c + ty*y;
		dst->m[5] = tyz + sx;

		dst->m[6] = txz + sy;
		dst->m[7] = tyz - sx;
		dst->m[8] = c + tz*z;
	}

	void Matrix3::CreateRotationX(float angle, Matrix3* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		float c = cos(angle);
		float s = sin(angle);

		dst->m[4] = c;
		dst->m[5] = s;
		dst->m[7] = -s;
		dst->m[8] = c;
	}

	void Matrix3::CreateRotationY(float angle, Matrix3* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		float c = cos(angle);
		float s = sin(angle);

		dst->m[0] = c;
		dst->m[2] = -s;
		dst->m[6] = s;
		dst->m[8] = c;
	}

	void Matrix3::CreateRotationZ(float angle, Matrix3* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);

		float c = cos(angle);
		float s = sin(angle);

		dst->m[0] = c;
		dst->m[1] = s;
		dst->m[3] = -s;
		dst->m[4] = c;
	}

	void Matrix3::CreateFromEuler(float yaw, float pitch, float roll, Matrix3* dst)
	{
		SCYTHE_ASSERT(dst);

		memcpy(dst, kMatrixIdentity, kMatrixSize);
		
		dst->RotateY(yaw);
		dst->RotateX(pitch);
		dst->RotateZ(roll);
	}

	void Matrix3::CreateNormal(const Matrix4& modelview, Matrix3* dst)
	{
		SCYTHE_ASSERT(dst);

		// Normal matrix is an inverted transposed modelview matrix
		Matrix4 itm;
		modelview.Invert(&itm);
		itm.Transpose();

		dst->m[0] = itm.m[0];
		dst->m[1] = itm.m[1];
		dst->m[2] = itm.m[2];
		dst->m[3] = itm.m[4];
		dst->m[4] = itm.m[5];
		dst->m[5] = itm.m[6];
		dst->m[6] = itm.m[8];
		dst->m[7] = itm.m[9];
		dst->m[8] = itm.m[10];
	}

	void Matrix3::Add(float scalar)
	{
		Add(scalar, this);
	}

	void Matrix3::Add(float scalar, Matrix3* dst)
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
	}

	void Matrix3::Add(const Matrix3& m)
	{
		Add(*this, m, this);
	}

	void Matrix3::Add(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
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
	}

	bool Matrix3::Decompose(Quaternion* rotation) const
	{
		// Nothing left to do.
		if (rotation == nullptr)
			return true;

		// Extract the scale.
		// This is simply the length of each axis (row/column) in the matrix.
		Vector3 xaxis(m[0], m[1], m[2]);
		float scaleX = xaxis.Length();

		Vector3 yaxis(m[3], m[4], m[5]);
		float scaleY = yaxis.Length();

		Vector3 zaxis(m[6], m[7], m[8]);
		float scaleZ = zaxis.Length();

		// Determine if we have a negative scale (true if determinant is less than zero).
		// In this case, we simply negate a single axis of the scale.
		float det = Determinant();
		if (det < 0.0f)
			scaleZ = -scaleZ;

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

	float Matrix3::Determinant() const
	{
		// Calculate the determinant.
		return (m[0] * m[4] * m[8]) + (m[1] * m[5] * m[6]) + (m[2] * m[3] * m[7])
			- (m[2] * m[4] * m[6]) - (m[1] * m[3] * m[8]) - (m[0] * m[5] * m[7]);
	}

	bool Matrix3::GetRotation(Quaternion* rotation) const
	{
		return Decompose(rotation);
	}

	void Matrix3::GetUpVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

		// Use +Y as up
		dst->x = m[3];
		dst->y = m[4];
		dst->z = m[5];
	}

	void Matrix3::GetDownVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);
		
		// Use -Y as down
		dst->x = -m[3];
		dst->y = -m[4];
		dst->z = -m[5];
	}

	void Matrix3::GetLeftVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use -X as left
		dst->x = -m[0];
		dst->y = -m[1];
		dst->z = -m[2];
#else
		// Use -Z as left
		dst->x = -m[6];
		dst->y = -m[7];
		dst->z = -m[8];
#endif
	}

	void Matrix3::GetRightVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use +X as right
		dst->x = m[0];
		dst->y = m[1];
		dst->z = m[2];
#else
		// Use +Z as right
		dst->x = m[6];
		dst->y = m[7];
		dst->z = m[8];
#endif
	}

	void Matrix3::GetForwardVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use -Z as forward
		dst->x = -m[6];
		dst->y = -m[7];
		dst->z = -m[8];
#else
		// Use +X as forward
		dst->x = m[0];
		dst->y = m[1];
		dst->z = m[2];
#endif
	}

	void Matrix3::GetBackVector(Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

#ifdef SCYTHE_ORIENTATION_Z
		// Use +Z as backward
		dst->x = m[6];
		dst->y = m[7];
		dst->z = m[8];
#else
		// Use -X as backward
		dst->x = -m[0];
		dst->y = -m[1];
		dst->z = -m[2];
#endif
	}

	bool Matrix3::Invert()
	{
		return Invert(this);
	}

	bool Matrix3::Invert(Matrix3* dst) const
	{
		float p0 = m[4] * m[8] - m[7] * m[5];
		float p1 = m[5] * m[6] - m[3] * m[8];
		float p2 = m[3] * m[7] - m[4] * m[6];

		float det = m[0] * p0 + m[1] * p1 + m[2] * p2;

		// Close to zero, can't invert.
		if (fabs(det) <= kFloatTolerance)
			return false;

		float inv_det = 1.0f / det;

		dst->m[0] = (p0) * inv_det;
		dst->m[1] = (m[7] * m[2] - m[1] * m[8]) * inv_det;
		dst->m[2] = (m[1] * m[5] - m[4] * m[2]) * inv_det;
		dst->m[3] = (p1) * inv_det;
		dst->m[4] = (m[0] * m[8] - m[6] * m[2]) * inv_det;
		dst->m[5] = (m[3] * m[2] - m[0] * m[5]) * inv_det;
		dst->m[6] = (p2) * inv_det;
		dst->m[7] = (m[6] * m[1] - m[0] * m[7]) * inv_det;
		dst->m[8] = (m[0] * m[4] - m[1] * m[3]) * inv_det;

		return true;
	}

	bool Matrix3::IsIdentity() const
	{
		return (memcmp(m, kMatrixIdentity, kMatrixSize) == 0);
	}

	void Matrix3::Multiply(float scalar)
	{
		Multiply(scalar, this);
	}

	void Matrix3::Multiply(float scalar, Matrix3* dst) const
	{
		Multiply(*this, scalar, dst);
	}

	void Matrix3::Multiply(const Matrix3& m, float scalar, Matrix3* dst)
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
	}

	void Matrix3::Multiply(const Matrix3& m)
	{
		Multiply(*this, m, this);
	}

	void Matrix3::Multiply(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
	{
		SCYTHE_ASSERT(dst);

		// Support the case where m1 or m2 is the same array as dst.
		float product[9];

		product[0] = m1.m[0] * m2.m[0] + m1.m[3] * m2.m[1] + m1.m[6] * m2.m[2];
		product[1] = m1.m[0] * m2.m[3] + m1.m[3] * m2.m[4] + m1.m[6] * m2.m[5];
		product[2] = m1.m[0] * m2.m[6] + m1.m[3] * m2.m[7] + m1.m[6] * m2.m[8];
		product[3] = m1.m[1] * m2.m[0] + m1.m[4] * m2.m[1] + m1.m[7] * m2.m[2];
		product[4] = m1.m[1] * m2.m[3] + m1.m[4] * m2.m[4] + m1.m[7] * m2.m[5];
		product[5] = m1.m[1] * m2.m[6] + m1.m[4] * m2.m[7] + m1.m[7] * m2.m[8];
		product[6] = m1.m[2] * m2.m[0] + m1.m[5] * m2.m[1] + m1.m[8] * m2.m[2];
		product[7] = m1.m[2] * m2.m[3] + m1.m[5] * m2.m[4] + m1.m[8] * m2.m[5];
		product[8] = m1.m[2] * m2.m[6] + m1.m[5] * m2.m[7] + m1.m[8] * m2.m[8];

		memcpy(dst->m, product, kMatrixSize);
	}

	void Matrix3::Negate()
	{
		Negate(this);
	}

	void Matrix3::Negate(Matrix3* dst) const
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
	}

	void Matrix3::Rotate(const Quaternion& q)
	{
		Rotate(q, this);
	}

	void Matrix3::Rotate(const Quaternion& q, Matrix3* dst) const
	{
		Matrix3 r;
		CreateRotation(q, &r);
		Multiply(*this, r, dst);
	}

	void Matrix3::Rotate(const Vector3& axis, float angle)
	{
		Rotate(axis, angle, this);
	}

	void Matrix3::Rotate(const Vector3& axis, float angle, Matrix3* dst) const
	{
		Matrix3 r;
		CreateRotation(axis, angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix3::RotateX(float angle)
	{
		RotateX(angle, this);
	}

	void Matrix3::RotateX(float angle, Matrix3* dst) const
	{
		Matrix3 r;
		CreateRotationX(angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix3::RotateY(float angle)
	{
		RotateY(angle, this);
	}

	void Matrix3::RotateY(float angle, Matrix3* dst) const
	{
		Matrix3 r;
		CreateRotationY(angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix3::RotateZ(float angle)
	{
		RotateZ(angle, this);
	}

	void Matrix3::RotateZ(float angle, Matrix3* dst) const
	{
		Matrix3 r;
		CreateRotationZ(angle, &r);
		Multiply(*this, r, dst);
	}

	void Matrix3::Set(float m11, float m12, float m13, float m21, float m22, float m23,
			 float m31, float m32, float m33)
	{
		m[0]  = m11;
		m[1]  = m21;
		m[2]  = m31;
		m[3]  = m12;
		m[4]  = m22;
		m[5]  = m32;
		m[6]  = m13;
		m[7]  = m23;
		m[8]  = m33;
	}

	void Matrix3::Set(const float* m)
	{
		SCYTHE_ASSERT(m);
		memcpy(this->m, m, kMatrixSize);
	}

	void Matrix3::Set(const Matrix3& m)
	{
		memcpy(this->m, m.m, kMatrixSize);
	}

	void Matrix3::SetIdentity()
	{
		memcpy(m, kMatrixIdentity, kMatrixSize);
	}

	void Matrix3::SetZero()
	{
		memset(m, 0, kMatrixSize);
	}

	void Matrix3::Subtract(const Matrix3& m)
	{
		Subtract(*this, m, this);
	}

	void Matrix3::Subtract(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
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
	}

	void Matrix3::TransformVector(Vector3* vector) const
	{
		SCYTHE_ASSERT(vector);
		TransformVector(*vector, vector);
	}

	void Matrix3::TransformVector(const Vector3& vector, Vector3* dst) const
	{
		SCYTHE_ASSERT(dst);

		const float* v = &vector.x;
		// Handle case where v == dst.
		float x = v[0] * m[0] + v[1] * m[3] + v[2] * m[6];
		float y = v[0] * m[1] + v[1] * m[4] + v[2] * m[7];
		float z = v[0] * m[2] + v[1] * m[5] + v[2] * m[8];

		dst->x = x;
		dst->y = y;
		dst->z = z;
	}

	void Matrix3::Transpose()
	{
		Transpose(this);
	}

	void Matrix3::Transpose(Matrix3* dst) const
	{
		SCYTHE_ASSERT(dst);

		float t[9] = {
			m[0], m[3], m[6],
			m[1], m[4], m[7],
			m[2], m[5], m[8]
		};
		memcpy(dst->m, t, kMatrixSize);
	}

} // namespace scythe