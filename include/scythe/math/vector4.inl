#include "vector4.h"

namespace scythe {
	
	inline const Vector4 Vector4::operator+(const Vector4& v) const
	{
		Vector4 result(*this);
		result.Add(v);
		return result;
	}

	inline Vector4& Vector4::operator+=(const Vector4& v)
	{
		Add(v);
		return *this;
	}

	inline const Vector4 Vector4::operator-(const Vector4& v) const
	{
		Vector4 result(*this);
		result.Subtract(v);
		return result;
	}

	inline Vector4& Vector4::operator-=(const Vector4& v)
	{
		Subtract(v);
		return *this;
	}

	inline const Vector4 Vector4::operator-() const
	{
		Vector4 result(*this);
		result.Negate();
		return result;
	}

	inline const Vector4 Vector4::operator*(float x) const
	{
		Vector4 result(*this);
		result.Scale(x);
		return result;
	}

	inline const Vector4 Vector4::operator*(const Vector4& v) const
	{
		Vector4 result(*this);
		result.Scale(v);
		return result;
	}

	inline Vector4& Vector4::operator*=(float x)
	{
		Scale(x);
		return *this;
	}

	inline Vector4& Vector4::operator*=(const Vector4& v)
	{
		Scale(v);
		return *this;
	}

	inline const Vector4 Vector4::operator/(const float x) const
	{
		return Vector4(this->x / x, this->y / x, this->z / x, this->w / x);
	}

	inline bool Vector4::operator<(const Vector4& v) const
	{
		if (x == v.x)
		{
			if (y == v.y)
			{
				if (z == v.z)
				{
					return w < v.w;
				}
				return z < v.z;
			}
			return y < v.y;
		}
		return x < v.x;
	}

	inline bool Vector4::operator==(const Vector4& v) const
	{
		return x==v.x && y==v.y && z==v.z && w==v.w;
	}

	inline bool Vector4::operator!=(const Vector4& v) const
	{
		return x!=v.x || y!=v.y || z!=v.z || w!=v.w;
	}

	inline Vector4::operator float *()
	{
		return &x;
	}

	inline Vector4::operator const float *() const
	{
		return &x;
	}

	inline const Vector4 operator*(float x, const Vector4& v)
	{
		Vector4 result(v);
		result.Scale(x);
		return result;
	}

} // namespace scythe