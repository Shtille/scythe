#include "vector3.h"

namespace scythe {
	
	inline const Vector3 Vector3::operator+(const Vector3& v) const
	{
		Vector3 result(*this);
		result.Add(v);
		return result;
	}

	inline Vector3& Vector3::operator+=(const Vector3& v)
	{
		Add(v);
		return *this;
	}

	inline const Vector3 Vector3::operator-(const Vector3& v) const
	{
		Vector3 result(*this);
		result.Subtract(v);
		return result;
	}

	inline Vector3& Vector3::operator-=(const Vector3& v)
	{
		Subtract(v);
		return *this;
	}

	inline const Vector3 Vector3::operator-() const
	{
		Vector3 result(*this);
		result.Negate();
		return result;
	}

	inline const Vector3 Vector3::operator*(float x) const
	{
		Vector3 result(*this);
		result.Scale(x);
		return result;
	}

	inline const Vector3 Vector3::operator*(const Vector3& v) const
	{
		Vector3 result(*this);
		result.Scale(v);
		return result;
	}

	inline Vector3& Vector3::operator*=(float x)
	{
		Scale(x);
		return *this;
	}

	inline Vector3& Vector3::operator*=(const Vector3& v)
	{
		Scale(v);
		return *this;
	}

	inline const Vector3 Vector3::operator/(const float x) const
	{
		return Vector3(this->x / x, this->y / x, this->z / x);
	}

	inline bool Vector3::operator<(const Vector3& v) const
	{
		if (x == v.x)
		{
			if (y == v.y)
			{
				return z < v.z;
			}
			return y < v.y;
		}
		return x < v.x;
	}

	inline bool Vector3::operator==(const Vector3& v) const
	{
		return x==v.x && y==v.y && z==v.z;
	}

	inline bool Vector3::operator!=(const Vector3& v) const
	{
		return x!=v.x || y!=v.y || z!=v.z;
	}

	inline Vector3::operator float *()
	{
		return &x;
	}

	inline Vector3::operator const float *() const
	{
		return &x;
	}

	inline const Vector3 operator*(float x, const Vector3& v)
	{
		Vector3 result(v);
		result.Scale(x);
		return result;
	}

	inline const float operator &(const Vector3& v1, const Vector3& v2)
	{
		return Vector3::Dot(v1, v2);
	}

	inline const Vector3 operator ^(const Vector3& v1, const Vector3& v2)
	{
		Vector3 result(v1);
		result.Cross(v2);
		return result;
	}

} // namespace scythe