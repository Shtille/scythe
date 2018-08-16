#include "quaternion.h"
#include "matrix.h"

#include <cmath>

namespace scythe {

	Quaternion::Quaternion()
	{

	}
	Quaternion::Quaternion(const float ix, const float iy, const float iz, const float iw)
	{
		x = ix;
		y = iy;
		z = iz;
		w = iw;
	}
	Quaternion::Quaternion(const Vector3& vector, const float angle)
	{
		Set(vector, angle);
	}
	Quaternion::Quaternion(const Vector3& from, const Vector3& to)
	{
		Set(from, to);
	}
	Quaternion& Quaternion::operator = (const Quaternion& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		return *this;
	}
	void Quaternion::operator *= (const float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}
	void Quaternion::operator /= (const float s)
	{
		float r = 1.0f / s;
		x *= r;
		y *= r;
		z *= r;
		w *= r;
	}
	Quaternion operator + (const Quaternion &q1, const Quaternion &q2)
	{
		return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
	}
	Quaternion operator - (const Quaternion &q1, const Quaternion &q2)
	{
		return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
	}
	Quaternion operator - (const Quaternion &q)
	{
		return Quaternion(-q.x, -q.y, -q.z, -q.w);
	}
	Quaternion operator * (const Quaternion &q1, const Quaternion &q2)
	{
		Vector3 v1 = Vector3(q1.x, q1.y, q1.z);
		Vector3 v2 = Vector3(q2.x, q2.y, q2.z);
		Vector3 v = (v1^v2) + q1.w*v2 + q2.w*v1;
		return Quaternion(v.x, v.y, v.z, q1.w*q2.w - (v1&v2));
	}
	Quaternion operator * (const float s, const Quaternion &q)
	{
		return Quaternion(s * q.x, s * q.y, s * q.z, s * q.w);
	}
	Quaternion operator * (const Quaternion &q, const float s)
	{
		return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
	}
	Quaternion operator / (const Quaternion &q, const float s)
	{
		float r = 1.0f / s;
		return Quaternion(q.x * r, q.y * r, q.z * r, q.w * r);
	}
	float operator & (const Quaternion &q1, const Quaternion &q2)
	{
		return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
	}
	bool operator == (const Quaternion &q1, const Quaternion &q2)
	{
		return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
	}
	bool operator != (const Quaternion &q1, const Quaternion &q2)
	{
		return q1.x != q2.x || q1.y != q2.y || q1.z != q2.z || q1.w != q2.w;
	}
	Vector3 Quaternion::xyz() const
	{
		return Vector3(x, y, z);
	}
	Vector3 Quaternion::Vector() const
	{
		return Vector3(x, y, z) / sqrtf(1.0f - w*w);
	}
	Vector3 Quaternion::Direction() const
	{
		float y2, z2, yy, zz, xy, xz, wy, wz;
		y2 = y + y;
		z2 = z + z;
		xy = x * y2;  xz = x * z2;
		yy = y * y2;  zz = z * z2;
		wy = w * y2;  wz = w * z2;
		return Vector3(1.0f - (yy + zz), xy + wz, xz - wy).GetNormalized();
	}
	void Quaternion::Null()
	{
		x = y = z = w = 0.0f;
	}
	float Quaternion::Length() const
	{
		return sqrtf(x*x + y*y + z*z + w*w);
	}
	float Quaternion::Norm() const
	{
		return x*x + y*y + z*z + w*w;
	}
	float Quaternion::Angle() const
	{
		if (w < 0.0f)
			return 2.0f*atan2f(-sqrtf(x*x + y*y + z*z), -w);
		else
			return 2.0f*atan2f(sqrtf(x*x + y*y + z*z), w);
	}
	float Quaternion::HalfAngle() const
	{
		if (w < 0.0f)
			return atan2f(-sqrtf(x*x + y*y + z*z), -w);
		else
			return atan2f(sqrtf(x*x + y*y + z*z), w);
	}
	void Quaternion::Identity()
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}
	void Quaternion::Set(const Quaternion& q)
	{
		(void)operator=(q);
	}
	void Quaternion::Set(const Vector3& vector, const float angle)
	{
		float halfangle = angle * 0.5f;
		float sin_ha = sinf(halfangle);
		x = vector.x * sin_ha;
		y = vector.y * sin_ha;
		z = vector.z * sin_ha;
		w = cosf(halfangle);
	}
	void Quaternion::Set(const Vector3& from, const Vector3& at)
	{
		Vector3 dir = (at - from).GetNormalized();
		Vector3 up, side;
		if (dir != UNIT_Y)
		{
			side = dir ^ UNIT_Y;
			side.Normalize();
			up = side ^ dir;
		}
		else
		{
			up = -UNIT_X;
			side = UNIT_Z;
		}
		Matrix3 matrix(dir.x, dir.y, dir.z, up.x, up.y, up.z, side.x, side.y, side.z);
		matrix.ToQuaternion(this);
	}
	void Quaternion::SetFast(const Vector3& from, const Vector3& to)
	{
		Vector3 dir = to - from;
		dir.Normalize();
		float cos_angle = dir.x; // dir dot i
		if (fabs(cos_angle) < 0.999f) // not zero angle
		{
			float angle = acosf(cos_angle);
			// axis = i x dir
			Vector3 axis(0.0f, -dir.z, dir.y);
			axis.Normalize();
			Set(axis, angle);
		}
		else if (cos_angle > 0.0f) // angle = 0
		{
			x = y = z = 0.0f;
			w = 1.0f;
		}
		else // angle = 180 degrees
		{
			x = z = w = 0.0f;
			y = 1.0f;
		}
	}
	void Quaternion::Normalize()
	{
		float r = 1.0f / Length();
		x *= r;
		y *= r;
		z *= r;
		w *= r;
	}
	Quaternion Quaternion::GetNormalized() const
	{
		return (*this) / Length();
	}
	Quaternion Quaternion::GetConjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}
	Quaternion Quaternion::GetInverse() const
	{
		return GetConjugate() / Norm();
	}
	void Quaternion::ToMatrix(Matrix3 * matrix) const
	{
		float x2, y2, z2, xx, yy, zz, xy, xz, yz, wx, wy, wz;
		x2 = x + x;
		y2 = y + y;
		z2 = z + z;
		xx = x * x2;  xy = x * y2;  xz = x * z2;
		yy = y * y2;  yz = y * z2;  zz = z * z2;
		wx = w * x2;  wy = w * y2;  wz = w * z2;

		matrix->a[0][0] = 1.0f - (yy + zz);
		matrix->a[0][1] = xy + wz;
		matrix->a[0][2] = xz - wy;

		matrix->a[1][0] = xy - wz;
		matrix->a[1][1] = 1.0f - (xx + zz);
		matrix->a[1][2] = yz + wx;

		matrix->a[2][0] = xz + wy;
		matrix->a[2][1] = yz - wx;
		matrix->a[2][2] = 1.0f - (xx + yy);
	}
	Vector3 Quaternion::RotateVector(const Vector3& _v) const
	{
		return ((*this) * Quaternion(_v.x, _v.y, _v.z, 0.0f) * GetInverse()).xyz();
	}
	void Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t,
		Quaternion * out)
	{
		Quaternion new_q2;
		float cos_om, scale0, scale1;
		cos_om = q1 & q2;
		if (cos_om < 0.0f)
		{
			cos_om = -cos_om;
			new_q2 = -q2;
		}
		else
		{
			new_q2 = q2;
		}
		if (cos_om < 0.9999f)
		{
			float omega = acosf(cos_om);
			float sin_om = sinf(omega);
			scale0 = sinf((1.0f - t) * omega) / sin_om;
			scale1 = sinf(t * omega) / sin_om;
		}
		else // small angle - LERP
		{
			scale0 = 1.0f - t;
			scale1 = t;
		}
		*out = (scale0 * q1) + (scale1 * new_q2);
		out->Normalize();
	}

} // namespace scythe