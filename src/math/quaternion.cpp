#include <scythe/math/quaternion.h>

#include <cmath>
#include <cstring>

#include <scythe/math/matrix4.h>
#include <scythe/math/constants.h>
#include <scythe/defines.h>

namespace scythe {

	Quaternion::Quaternion()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, w(1.0f)
	{
	}

	Quaternion::Quaternion(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
	{
	}

	Quaternion::Quaternion(const float* array)
	{
		Set(array);
	}

	Quaternion::Quaternion(const Matrix4& m)
	{
		Set(m);
	}

	Quaternion::Quaternion(const Vector3& axis, float angle)
	{
		Set(axis, angle);
	}

	Quaternion::Quaternion(const Quaternion& copy)
	{
		Set(copy);
	}

	Quaternion::~Quaternion()
	{
	}

	const Quaternion& Quaternion::Identity()
	{
		static Quaternion value(0.0f, 0.0f, 0.0f, 1.0f);
		return value;
	}

	const Quaternion& Quaternion::Zero()
	{
		static Quaternion value(0.0f, 0.0f, 0.0f, 0.0f);
		return value;
	}

	bool Quaternion::IsIdentity() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f;
	}

	bool Quaternion::IsZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
	}

	void Quaternion::CreateFromEuler(float yaw, float pitch, float roll, Quaternion* dst)
	{
		SCYTHE_ASSERT(dst);

		pitch *= 0.5f;
		yaw *= 0.5f;
		roll *= 0.5f;

		float sinp = sin(pitch);
		float siny = sin(yaw);
		float sinr = sin(roll);
		float cosp = cos(pitch);
		float cosy = cos(yaw);
		float cosr = cos(roll);

		dst->w = cosp * cosy * cosr + sinp * siny * sinr;
		dst->x = sinp * cosy * cosr - cosp * siny * sinr;
		dst->y = cosp * siny * cosr + sinp * cosy * sinr;
		dst->z = cosp * cosy * sinr - sinp * siny * cosr;
	}

	void Quaternion::CreateFromRotationMatrix(const Matrix4& m, Quaternion* dst)
	{
		m.GetRotation(dst);
	}

	void Quaternion::CreateFromAxisAngle(const Vector3& axis, float angle, Quaternion* dst)
	{
		SCYTHE_ASSERT(dst);

		float halfAngle = angle * 0.5f;
		float sinHalfAngle = sinf(halfAngle);

		Vector3 normal(axis);
		normal.Normalize();
		dst->x = normal.x * sinHalfAngle;
		dst->y = normal.y * sinHalfAngle;
		dst->z = normal.z * sinHalfAngle;
		dst->w = cosf(halfAngle);
	}

	void Quaternion::ComputeEuler(float* yaw, float* pitch, float* roll)
	{
		SCYTHE_ASSERT(yaw);
		SCYTHE_ASSERT(pitch);
		SCYTHE_ASSERT(roll);

		*pitch = std::atan2(2 * (w*x + y*z), 1 - 2 * (x*x + y*y));
		*yaw = std::asin(2 * (w*y - z*x));
		*roll = std::atan2(2 * (w*z + x*y), 1 - 2 * (y*y + z*z));
	}

	void Quaternion::Conjugate()
	{
		Conjugate(this);
	}

	void Quaternion::Conjugate(Quaternion* dst) const
	{
		SCYTHE_ASSERT(dst);

		dst->x = -x;
		dst->y = -y;
		dst->z = -z;
		dst->w =  w;
	}

	bool Quaternion::Inverse()
	{
		return Inverse(this);
	}

	bool Quaternion::Inverse(Quaternion* dst) const
	{
		SCYTHE_ASSERT(dst);

		float n = x * x + y * y + z * z + w * w;
		if (n == 1.0f)
		{
			dst->x = -x;
			dst->y = -y;
			dst->z = -z;
			dst->w = w;

			return true;
		}

		// Too close to zero.
		if (n < kFloatTolerance)
			return false;

		n = 1.0f / n;
		dst->x = -x * n;
		dst->y = -y * n;
		dst->z = -z * n;
		dst->w = w * n;

		return true;
	}

	void Quaternion::Multiply(const Quaternion& q)
	{
		Multiply(*this, q, this);
	}

	void Quaternion::Multiply(const Quaternion& q1, const Quaternion& q2, Quaternion* dst)
	{
		SCYTHE_ASSERT(dst);

		float x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
		float y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
		float z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
		float w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

		dst->x = x;
		dst->y = y;
		dst->z = z;
		dst->w = w;
	}

	void Quaternion::Normalize()
	{
		Normalize(this);
	}

	void Quaternion::Normalize(Quaternion* dst) const
	{
		SCYTHE_ASSERT(dst);

		if (this != dst)
		{
			dst->x = x;
			dst->y = y;
			dst->z = z;
			dst->w = w;
		}

		float n = x * x + y * y + z * z + w * w;

		// Already normalized.
		if (n == 1.0f)
			return;

		n = sqrt(n);
		// Too close to zero.
		if (n < kFloatTolerance)
			return;

		n = 1.0f / n;
		dst->x *= n;
		dst->y *= n;
		dst->z *= n;
		dst->w *= n;
	}

	void Quaternion::RotatePoint(const Vector3& point, Vector3* dst) const
	{
		Quaternion vecQuat;
		Quaternion resQuat;
		vecQuat.x = point.x;
		vecQuat.y = point.y;
		vecQuat.z = point.z;
		vecQuat.w = 0.0f;

		Quaternion conQuat;
		this->Conjugate(&conQuat);

		resQuat = vecQuat * conQuat;
		resQuat = (*this) * resQuat;

		dst->Set(resQuat.x, resQuat.y, resQuat.z);
	}

	void Quaternion::Set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Quaternion::Set(const float* array)
	{
		SCYTHE_ASSERT(array);

		x = array[0];
		y = array[1];
		z = array[2];
		w = array[3];
	}

	void Quaternion::Set(const Matrix4& m)
	{
		Quaternion::CreateFromRotationMatrix(m, this);
	}

	void Quaternion::Set(const Vector3& axis, float angle)
	{
		Quaternion::CreateFromAxisAngle(axis, angle, this);
	}

	void Quaternion::Set(const Vector3& from, const Vector3& at)
	{
		Vector3 dir = at - from;
		Vector3 up, side;
		dir.Normalize();
		if (dir != Vector3::UnitY())
		{
			side = dir ^ Vector3::UnitY();
			side.Normalize();
			up = side ^ dir;
		}
		else
		{
			up = -Vector3::UnitX();
			side = Vector3::UnitZ();
		}
		Matrix4 matrix;
		Matrix4::CreateRotation(dir, up, side, &matrix);
		Quaternion::CreateFromRotationMatrix(matrix, this);
	}

	void Quaternion::Set(const Quaternion& q)
	{
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
	}

	void Quaternion::SetIdentity()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	float Quaternion::ToAxisAngle(Vector3* axis) const
	{
		Quaternion q(x, y, z, w);
		q.Normalize();
		if (axis)
		{
			axis->x = q.x;
			axis->y = q.y;
			axis->z = q.z;
			axis->Normalize();
		}
		return (2.0f * acos(q.w));
	}

	void Quaternion::GetDirection(Vector3 * dst) const
	{
		SCYTHE_ASSERT(dst);

		// Should coincide with Matrix4.GetForwardVector()
		float x2 = x + x;
		float y2 = y + y;
		float z2 = z + z;

#ifdef SCYTHE_ORIENTATION_Z
		// We use -Z as forward direction.
		float xx2 = x * x2;
		float yy2 = y * y2;
		// float zz2 = z * z2;
		// float xy2 = x * y2;
		float xz2 = x * z2;
		float yz2 = y * z2;
		float wx2 = w * x2;
		float wy2 = w * y2;
		// float wz2 = w * z2;

		dst->x = -(xz2 + wy2);
		dst->y = -(yz2 - wx2);
		dst->z = -(1.0f - xx2 - yy2);
#else
		// We use +X as forward direction.
		// float xx2 = x * x2;
		float yy2 = y * y2;
		float zz2 = z * z2;
		float xy2 = x * y2;
		float xz2 = x * z2;
		// float yz2 = y * z2;
		// float wx2 = w * x2;
		float wy2 = w * y2;
		float wz2 = w * z2;

		dst->x = 1.0f - yy2 - zz2;
		dst->y = xy2 + wz2;
		dst->z = xz2 - wy2;
#endif
		dst->Normalize();
	}

	void Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst)
	{
		SCYTHE_ASSERT(dst);
		SCYTHE_ASSERT(!(t < 0.0f || t > 1.0f));

		if (t == 0.0f)
		{
			memcpy(dst, &q1, sizeof(float) * 4);
			return;
		}
		else if (t == 1.0f)
		{
			memcpy(dst, &q2, sizeof(float) * 4);
			return;
		}

		float t1 = 1.0f - t;

		dst->x = t1 * q1.x + t * q2.x;
		dst->y = t1 * q1.y + t * q2.y;
		dst->z = t1 * q1.z + t * q2.z;
		dst->w = t1 * q1.w + t * q2.w;
	}

	void Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst)
	{
		SCYTHE_ASSERT(dst);
		Slerp(q1.x, q1.y, q1.z, q1.w, q2.x, q2.y, q2.z, q2.w, t, &dst->x, &dst->y, &dst->z, &dst->w);
	}

	void Quaternion::Squad(const Quaternion& q1, const Quaternion& q2, const Quaternion& s1, const Quaternion& s2, float t, Quaternion* dst)
	{
		SCYTHE_ASSERT(!(t < 0.0f || t > 1.0f));

		Quaternion dstQ(0.0f, 0.0f, 0.0f, 1.0f);
		Quaternion dstS(0.0f, 0.0f, 0.0f, 1.0f);

		SlerpForSquad(q1, q2, t, &dstQ);
		SlerpForSquad(s1, s2, t, &dstS);
		SlerpForSquad(dstQ, dstS, 2.0f * t * (1.0f - t), dst);
	}

	void Quaternion::Slerp(float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz, float* dstw)
	{
		// Fast slerp implementation by kwhatmough:
		// It contains no division operations, no trig, no inverse trig
		// and no sqrt. Not only does this code tolerate small constraint
		// errors in the input quaternions, it actually corrects for them.
		SCYTHE_ASSERT(dstx && dsty && dstz && dstw);
		SCYTHE_ASSERT(!(t < 0.0f || t > 1.0f));

		if (t == 0.0f)
		{
			*dstx = q1x;
			*dsty = q1y;
			*dstz = q1z;
			*dstw = q1w;
			return;
		}
		else if (t == 1.0f)
		{
			*dstx = q2x;
			*dsty = q2y;
			*dstz = q2z;
			*dstw = q2w;
			return;
		}

		if (q1x == q2x && q1y == q2y && q1z == q2z && q1w == q2w)
		{
			*dstx = q1x;
			*dsty = q1y;
			*dstz = q1z;
			*dstw = q1w;
			return;
		}

		float halfY, alpha, beta;
		float u, f1, f2a, f2b;
		float ratio1, ratio2;
		float halfSecHalfTheta, versHalfTheta;
		float sqNotU, sqU;

		float cosTheta = q1w * q2w + q1x * q2x + q1y * q2y + q1z * q2z;

		// As usual in all slerp implementations, we fold theta.
		alpha = cosTheta >= 0 ? 1.0f : -1.0f;
		halfY = 1.0f + alpha * cosTheta;

		// Here we bisect the interval, so we need to fold t as well.
		f2b = t - 0.5f;
		u = f2b >= 0 ? f2b : -f2b;
		f2a = u - f2b;
		f2b += u;
		u += u;
		f1 = 1.0f - u;

		// One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
		halfSecHalfTheta = 1.09f - (0.476537f - 0.0903321f * halfY) * halfY;
		halfSecHalfTheta *= 1.5f - halfY * halfSecHalfTheta * halfSecHalfTheta;
		versHalfTheta = 1.0f - halfY * halfSecHalfTheta;

		// Evaluate series expansions of the coefficients.
		sqNotU = f1 * f1;
		ratio2 = 0.0000440917108f * versHalfTheta;
		ratio1 = -0.00158730159f + (sqNotU - 16.0f) * ratio2;
		ratio1 = 0.0333333333f + ratio1 * (sqNotU - 9.0f) * versHalfTheta;
		ratio1 = -0.333333333f + ratio1 * (sqNotU - 4.0f) * versHalfTheta;
		ratio1 = 1.0f + ratio1 * (sqNotU - 1.0f) * versHalfTheta;

		sqU = u * u;
		ratio2 = -0.00158730159f + (sqU - 16.0f) * ratio2;
		ratio2 = 0.0333333333f + ratio2 * (sqU - 9.0f) * versHalfTheta;
		ratio2 = -0.333333333f + ratio2 * (sqU - 4.0f) * versHalfTheta;
		ratio2 = 1.0f + ratio2 * (sqU - 1.0f) * versHalfTheta;

		// Perform the bisection and resolve the folding done earlier.
		f1 *= ratio1 * halfSecHalfTheta;
		f2a *= ratio2;
		f2b *= ratio2;
		alpha *= f1 + f2a;
		beta = f1 + f2b;

		// Apply final coefficients to a and b as usual.
		float w = alpha * q1w + beta * q2w;
		float x = alpha * q1x + beta * q2x;
		float y = alpha * q1y + beta * q2y;
		float z = alpha * q1z + beta * q2z;

		// This final adjustment to the quaternion's length corrects for
		// any small constraint error in the inputs q1 and q2 But as you
		// can see, it comes at the cost of 9 additional multiplication
		// operations. If this error-correcting feature is not required,
		// the following code may be removed.
		f1 = 1.5f - 0.5f * (w * w + x * x + y * y + z * z);
		*dstw = w * f1;
		*dstx = x * f1;
		*dsty = y * f1;
		*dstz = z * f1;
	}

	void Quaternion::SlerpForSquad(const Quaternion& q1, const Quaternion& q2, float t, Quaternion* dst)
	{
		SCYTHE_ASSERT(dst);

		// cos(omega) = q1 * q2;
		// slerp(q1, q2, t) = (q1*sin((1-t)*omega) + q2*sin(t*omega))/sin(omega);
		// q1 = +- q2, slerp(q1,q2,t) = q1.
		// This is a straight-forward implementation of the formula of slerp. It does not do any sign switching.
		float c = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

		if (fabs(c) >= 1.0f)
		{
			dst->x = q1.x;
			dst->y = q1.y;
			dst->z = q1.z;
			dst->w = q1.w;
			return;
		}

		float omega = acos(c);
		float s = sqrt(1.0f - c * c);
		if (fabs(s) <= kFloatTolerance)
		{
			dst->x = q1.x;
			dst->y = q1.y;
			dst->z = q1.z;
			dst->w = q1.w;
			return;
		}

		float r1 = sin((1 - t) * omega) / s;
		float r2 = sin(t * omega) / s;
		dst->x = (q1.x * r1 + q2.x * r2);
		dst->y = (q1.y * r1 + q2.y * r2);
		dst->z = (q1.z * r1 + q2.z * r2);
		dst->w = (q1.w * r1 + q2.w * r2);
	}

} // namespace scythe