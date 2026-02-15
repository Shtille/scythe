#include "matrix3.h"

namespace scythe {
	
	inline const Matrix3 Matrix3::operator+(const Matrix3& m) const
	{
		Matrix3 result(*this);
		result.Add(m);
		return result;
	}

	inline Matrix3& Matrix3::operator+=(const Matrix3& m)
	{
		Add(m);
		return *this;
	}

	inline const Matrix3 Matrix3::operator-(const Matrix3& m) const
	{
		Matrix3 result(*this);
		result.Subtract(m);
		return result;
	}

	inline Matrix3& Matrix3::operator-=(const Matrix3& m)
	{
		Subtract(m);
		return *this;
	}

	inline const Matrix3 Matrix3::operator-() const
	{
		Matrix3 m(*this);
		m.Negate();
		return m;
	}

	inline const Matrix3 Matrix3::operator*(const Matrix3& m) const
	{
		Matrix3 result(*this);
		result.Multiply(m);
		return result;
	}

	inline Matrix3& Matrix3::operator*=(const Matrix3& m)
	{
		Multiply(m);
		return *this;
	}

	inline Matrix3::operator float *()
	{
		return m;
	}

	inline Matrix3::operator const float *() const
	{
		return m;
	}

	inline Vector3& operator*=(Vector3& v, const Matrix3& m)
	{
		m.TransformVector(&v);
		return v;
	}

	inline const Vector3 operator*(const Matrix3& m, const Vector3& v)
	{
		Vector3 x;
		m.TransformVector(v, &x);
		return x;
	}

} // namespace scythe