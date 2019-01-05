#include "vector2.h"

namespace scythe {
	
	inline const Vector2 Vector2::operator+(const Vector2& v) const
	{
		Vector2 result(*this);
		result.Add(v);
		return result;
	}

	inline Vector2& Vector2::operator+=(const Vector2& v)
	{
		Add(v);
		return *this;
	}

	inline const Vector2 Vector2::operator-(const Vector2& v) const
	{
		Vector2 result(*this);
		result.Subtract(v);
		return result;
	}

	inline Vector2& Vector2::operator-=(const Vector2& v)
	{
		Subtract(v);
		return *this;
	}

	inline const Vector2 Vector2::operator-() const
	{
		Vector2 result(*this);
		result.Negate();
		return result;
	}

	inline const Vector2 Vector2::operator*(float x) const
	{
		Vector2 result(*this);
		result.Scale(x);
		return result;
	}

	inline const Vector2 Vector2::operator*(const Vector2& v) const
	{
		Vector2 result(*this);
		result.Scale(v);
		return result;
	}

	inline Vector2& Vector2::operator*=(float x)
	{
		Scale(x);
		return *this;
	}

	inline Vector2& Vector2::operator*=(const Vector2& v)
	{
		Scale(v);
		return *this;
	}

	inline const Vector2 Vector2::operator/(const float x) const
	{
		return Vector2(this->x / x, this->y / x);
	}

	inline bool Vector2::operator<(const Vector2& v) const
	{
		if (x == v.x)
		{
			return y < v.y;
		}
		return x < v.x;
	}

	inline bool Vector2::operator==(const Vector2& v) const
	{
		return x==v.x && y==v.y;
	}

	inline bool Vector2::operator!=(const Vector2& v) const
	{
		return x!=v.x || y!=v.y;
	}

	inline Vector2::operator float *()
	{
		return &x;
	}

	inline Vector2::operator const float *() const
	{
		return &x;
	}

	inline const Vector2 operator*(float x, const Vector2& v)
	{
		Vector2 result(v);
		result.Scale(x);
		return result;
	}

} // namespace scythe