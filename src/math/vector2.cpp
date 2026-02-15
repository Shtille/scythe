#include <scythe/math/vector2.h>

#include <cmath>

#include <scythe/math/constants.h>
#include <scythe/defines.h>

namespace scythe {

	Vector2::Vector2()
	: x(0.0f)
	, y(0.0f)
	{
	}

	Vector2::Vector2(float value)
	: x(value)
	, y(value)
	{
	}

	Vector2::Vector2(float x, float y)
	: x(x)
	, y(y)
	{
	}

	Vector2::Vector2(const float* array)
	{
		Set(array);
	}

	Vector2::Vector2(const Vector2& p1, const Vector2& p2)
	{
		Set(p1, p2);
	}

	Vector2::Vector2(const Vector2& copy)
	{
		Set(copy);
	}

	Vector2::~Vector2()
	{
	}

	const Vector2& Vector2::Zero()
	{
		static Vector2 value(0.0f, 0.0f);
		return value;
	}

	const Vector2& Vector2::One()
	{
		static Vector2 value(1.0f, 1.0f);
		return value;
	}

	const Vector2& Vector2::UnitX()
	{
		static Vector2 value(1.0f, 0.0f);
		return value;
	}

	const Vector2& Vector2::UnitY()
	{
		static Vector2 value(0.0f, 1.0f);
		return value;
	}

	bool Vector2::IsZero() const
	{
		return x == 0.0f && y == 0.0f;
	}

	bool Vector2::IsOne() const
	{
		return x == 1.0f && y == 1.0f;
	}

	float Vector2::Angle(const Vector2& v1, const Vector2& v2)
	{
		float dz = v1.x * v2.y - v1.y * v2.x;
		return atan2f(fabsf(dz) + kFloatSmall, Dot(v1, v2));
	}

	void Vector2::Add(const Vector2& v)
	{
		x += v.x;
		y += v.y;
	}

	void Vector2::Add(const Vector2& v1, const Vector2& v2, Vector2* dst)
	{
		SCYTHE_ASSERT(dst);

		dst->x = v1.x + v2.x;
		dst->y = v1.y + v2.y;
	}

	void Vector2::Clamp(const Vector2& min, const Vector2& max)
	{
		SCYTHE_ASSERT(!(min.x > max.x || min.y > max.y ));

		// Clamp the x value.
		if (x < min.x)
			x = min.x;
		if (x > max.x)
			x = max.x;

		// Clamp the y value.
		if (y < min.y)
			y = min.y;
		if (y > max.y)
			y = max.y;
	}

	void Vector2::Clamp(const Vector2& v, const Vector2& min, const Vector2& max, Vector2* dst)
	{
		SCYTHE_ASSERT(dst);
		SCYTHE_ASSERT(!(min.x > max.x || min.y > max.y ));

		// Clamp the x value.
		dst->x = v.x;
		if (dst->x < min.x)
			dst->x = min.x;
		if (dst->x > max.x)
			dst->x = max.x;

		// Clamp the y value.
		dst->y = v.y;
		if (dst->y < min.y)
			dst->y = min.y;
		if (dst->y > max.y)
			dst->y = max.y;
	}

	float Vector2::Distance(const Vector2& v) const
	{
		float dx = v.x - x;
		float dy = v.y - y;

		return sqrt(dx * dx + dy * dy);
	}

	float Vector2::DistanceSquared(const Vector2& v) const
	{
		float dx = v.x - x;
		float dy = v.y - y;
		return (dx * dx + dy * dy);
	}

	float Vector2::Dot(const Vector2& v) const
	{
		return (x * v.x + y * v.y);
	}

	float Vector2::Dot(const Vector2& v1, const Vector2& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y);
	}

	float Vector2::Length() const
	{
		return sqrt(x * x + y * y);
	}

	float Vector2::LengthSquared() const
	{
		return (x * x + y * y);
	}

	void Vector2::Negate()
	{
		x = -x;
		y = -y;
	}

	Vector2& Vector2::Normalize()
	{
		Normalize(this);
		return *this;
	}

	void Vector2::Normalize(Vector2* dst) const
	{
		SCYTHE_ASSERT(dst);

		if (dst != this)
		{
			dst->x = x;
			dst->y = y;
		}

		float n = x * x + y * y;
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
	}

	void Vector2::Scale(float scalar)
	{
		x *= scalar;
		y *= scalar;
	}

	void Vector2::Scale(const Vector2& scale)
	{
		x *= scale.x;
		y *= scale.y;
	}

	void Vector2::Rotate(const Vector2& point, float angle)
	{
		float sinAngle = sin(angle);
		float cosAngle = cos(angle);

		if (point.IsZero())
		{
			float tempX = x * cosAngle - y * sinAngle;
			y = y * cosAngle + x * sinAngle;
			x = tempX;
		}
		else
		{
			float tempX = x - point.x;
			float tempY = y - point.y;

			x = tempX * cosAngle - tempY * sinAngle + point.x;
			y = tempY * cosAngle + tempX * sinAngle + point.y;
		}
	}

	void Vector2::Set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void Vector2::Set(const float* array)
	{
		SCYTHE_ASSERT(array);

		x = array[0];
		y = array[1];
	}

	void Vector2::Set(const Vector2& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	void Vector2::Set(const Vector2& p1, const Vector2& p2)
	{
		 x = p2.x - p1.x;
		 y = p2.y - p1.y;
	}

	void Vector2::Subtract(const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
	}

	void Vector2::Subtract(const Vector2& v1, const Vector2& v2, Vector2* dst)
	{
		SCYTHE_ASSERT(dst);

		dst->x = v1.x - v2.x;
		dst->y = v1.y - v2.y;
	}

	void Vector2::Smooth(const Vector2& target, float elapsedTime, float responseTime)
	{
		if (elapsedTime > 0)
		{
			*this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
		}
	}

} // namespace scythe