#ifndef __SCYTHE_QUATERNION_H__
#define __SCYTHE_QUATERNION_H__

#include "vector.h"

namespace scythe {

	struct Matrix3;

	//! Single-precision quaternion
	struct Quaternion {

		Quaternion();
		Quaternion(const float ix, const float iy, const float iz, const float iw);
		Quaternion(const Vector3& vector, const float angle); //!< angle in radians
		Quaternion(const Vector3& from, const Vector3& to); //!< from 2 directional points

		Quaternion& operator = (const Quaternion& q);
		void operator *= (const float s);
		void operator /= (const float s);

		friend Quaternion operator + (const Quaternion &q1, const Quaternion &q2);
		friend Quaternion operator - (const Quaternion &q1, const Quaternion &q2);
		friend Quaternion operator - (const Quaternion &q);
		friend Quaternion operator * (const Quaternion &q1, const Quaternion &q2);
		friend Quaternion operator * (const float s, const Quaternion &q);
		friend Quaternion operator * (const Quaternion &q, const float s);
		friend Quaternion operator / (const Quaternion &q, const float s);
		friend float operator & (const Quaternion &q1, const Quaternion &q2);
		friend bool operator == (const Quaternion &q1, const Quaternion &q2);
		friend bool operator != (const Quaternion &q1, const Quaternion &q2);

		Vector3 xyz() const;
		Vector3 Vector() const;
		Vector3 Direction() const;

		void Null();
		float Length() const;
		float Norm() const;
		float Angle() const; // returns angle in [-Pi; Pi]
		float HalfAngle() const; // returns angle in [-Pi/2; Pi/2]
		void Identity();
		void Set(const Quaternion& q);
		void Set(const Vector3& vector, const float angle);
		void Set(const Vector3& from, const Vector3& to);
		void SetFast(const Vector3& from, const Vector3& to);
		void Normalize();
		Quaternion GetNormalized() const;
		Quaternion GetConjugate() const;
		Quaternion GetInverse() const;

		void ToMatrix(Matrix3 * matrix) const;
		Vector3 RotateVector(const Vector3& _v) const;
		
		static void Slerp(const Quaternion& q1, const Quaternion& q2, float t,
			Quaternion * out);

		float x, y, z, w;
	};

} // namespace scythe

typedef scythe::Quaternion quat;

#endif