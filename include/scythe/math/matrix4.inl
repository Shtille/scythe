#include "matrix4.h"

namespace scythe {
	
	inline const Matrix4 Matrix4::operator+(const Matrix4& m) const
	{
		Matrix4 result(*this);
		result.Add(m);
		return result;
	}

	inline Matrix4& Matrix4::operator+=(const Matrix4& m)
	{
		Add(m);
		return *this;
	}

	inline const Matrix4 Matrix4::operator-(const Matrix4& m) const
	{
		Matrix4 result(*this);
		result.Subtract(m);
		return result;
	}

	inline Matrix4& Matrix4::operator-=(const Matrix4& m)
	{
		Subtract(m);
		return *this;
	}

	inline const Matrix4 Matrix4::operator-() const
	{
		Matrix4 m(*this);
		m.Negate();
		return m;
	}

	inline const Matrix4 Matrix4::operator*(const Matrix4& m) const
	{
		Matrix4 result(*this);
		result.Multiply(m);
		return result;
	}

	inline Matrix4& Matrix4::operator*=(const Matrix4& m)
	{
		Multiply(m);
		return *this;
	}

	inline Matrix4::operator float *()
	{
		return m;
	}

	inline Matrix4::operator const float *() const
	{
		return m;
	}

	inline Vector3& operator*=(Vector3& v, const Matrix4& m)
	{
		m.TransformVector(&v);
		return v;
	}

	inline const Vector3 operator*(const Matrix4& m, const Vector3& v)
	{
		Vector3 x;
		m.TransformVector(v, &x);
		return x;
	}

	inline Vector4& operator*=(Vector4& v, const Matrix4& m)
	{
		m.TransformVector(&v);
		return v;
	}

	inline const Vector4 operator*(const Matrix4& m, const Vector4& v)
	{
		Vector4 x;
		m.TransformVector(v, &x);
		return x;
	}

} // namespace scythe