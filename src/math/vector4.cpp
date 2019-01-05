#include "vector4.h"

#include "constants.h"

#include "common/sc_assert.h"

#include <cmath>

namespace scythe {

	Vector4::Vector4()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, w(0.0f)
	{
	}

	Vector4::Vector4(float x)
	: x(x)
	, y(x)
	, z(x)
	, w(x)
	{
	}

	Vector4::Vector4(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
	{
	}

	Vector4::Vector4(const float* src)
	{
		Set(src);
	}

	Vector4::Vector4(const Vector4& p1, const Vector4& p2)
	{
		Set(p1, p2);
	}

	Vector4::Vector4(const Vector4& copy)
	{
		Set(copy);
	}

	Vector4 Vector4::FromColor(unsigned int color)
	{
		float components[4];
		int componentIndex = 0;
		for (int i = 3; i >= 0; --i)
		{
			int component = (color >> i*8) & 0x000000ff;

			components[componentIndex++] = static_cast<float>(component) / 255.0f;
		}

		Vector4 value(components);
		return value;
	}

	Vector4::~Vector4()
	{
	}

	const Vector4& Vector4::Zero()
	{
		static Vector4 value(0.0f, 0.0f, 0.0f, 0.0f);
		return value;
	}

	const Vector4& Vector4::One()
	{
		static Vector4 value(1.0f, 1.0f, 1.0f, 1.0f);
		return value;
	}

	const Vector4& Vector4::UnitX()
	{
		static Vector4 value(1.0f, 0.0f, 0.0f, 0.0f);
		return value;
	}

	const Vector4& Vector4::UnitY()
	{
		static Vector4 value(0.0f, 1.0f, 0.0f, 0.0f);
		return value;
	}

	const Vector4& Vector4::UnitZ()
	{
		static Vector4 value(0.0f, 0.0f, 1.0f, 0.0f);
		return value;
	}

	const Vector4& Vector4::UnitW()
	{
		static Vector4 value(0.0f, 0.0f, 0.0f, 1.0f);
		return value;
	}

	bool Vector4::IsZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
	}

	bool Vector4::IsOne() const
	{
		return x == 1.0f && y == 1.0f && z == 1.0f && w == 1.0f;
	}

	float Vector4::Angle(const Vector4& v1, const Vector4& v2)
	{
		float dx = v1.w * v2.x - v1.x * v2.w - v1.y * v2.z + v1.z * v2.y;
		float dy = v1.w * v2.y - v1.y * v2.w - v1.z * v2.x + v1.x * v2.z;
		float dz = v1.w * v2.z - v1.z * v2.w - v1.x * v2.y + v1.y * v2.x;

		return atan2f(sqrt(dx * dx + dy * dy + dz * dz) + kFloatSmall, Dot(v1, v2));
	}

	void Vector4::Add(const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	void Vector4::Add(const Vector4& v1, const Vector4& v2, Vector4* dst)
	{
		SC_ASSERT(dst);

		dst->x = v1.x + v2.x;
		dst->y = v1.y + v2.y;
		dst->z = v1.z + v2.z;
		dst->w = v1.w + v2.w;
	}

	void Vector4::Clamp(const Vector4& min, const Vector4& max)
	{
		SC_ASSERT(!(min.x > max.x || min.y > max.y || min.z > max.z || min.w > max.w));

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

		// Clamp the z value.
		if (z < min.z)
			z = min.z;
		if (z > max.z)
			z = max.z;

		// Clamp the z value.
		if (w < min.w)
			w = min.w;
		if (w > max.w)
			w = max.w;
	}

	void Vector4::Clamp(const Vector4& v, const Vector4& min, const Vector4& max, Vector4* dst)
	{
		SC_ASSERT(dst);
		SC_ASSERT(!(min.x > max.x || min.y > max.y || min.z > max.z || min.w > max.w));

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

		// Clamp the z value.
		dst->z = v.z;
		if (dst->z < min.z)
			dst->z = min.z;
		if (dst->z > max.z)
			dst->z = max.z;

		// Clamp the w value.
		dst->w = v.w;
		if (dst->w < min.w)
			dst->w = min.w;
		if (dst->w > max.w)
			dst->w = max.w;
	}

	float Vector4::Distance(const Vector4& v) const
	{
		float dx = v.x - x;
		float dy = v.y - y;
		float dz = v.z - z;
		float dw = v.w - w;

		return sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
	}

	float Vector4::DistanceSquared(const Vector4& v) const
	{
		float dx = v.x - x;
		float dy = v.y - y;
		float dz = v.z - z;
		float dw = v.w - w;

		return (dx * dx + dy * dy + dz * dz + dw * dw);
	}

	float Vector4::Dot(const Vector4& v) const
	{
		return (x * v.x + y * v.y + z * v.z + w * v.w);
	}

	float Vector4::Dot(const Vector4& v1, const Vector4& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
	}

	float Vector4::Length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}


	float Vector4::LengthSquared() const
	{
		return (x * x + y * y + z * z + w * w);
	}

	void Vector4::Negate()
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
	}

	Vector4& Vector4::Normalize()
	{
		Normalize(this);
		return *this;
	}

	void Vector4::Normalize(Vector4* dst) const
	{
		SC_ASSERT(dst);

		if (dst != this)
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

	void Vector4::Scale(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
	}

	void Vector4::Scale(const Vector4& scale)
	{
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
		w *= scale.w;
	}

	void Vector4::Set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Vector4::Set(const float* array)
	{
		SC_ASSERT(array);

		x = array[0];
		y = array[1];
		z = array[2];
		w = array[3];
	}

	void Vector4::Set(const Vector4& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;
	}

	void Vector4::Set(const Vector4& p1, const Vector4& p2)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
		z = p2.z - p1.z;
		w = p2.w - p1.w;
	}

	void Vector4::Subtract(const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
	}

	void Vector4::Subtract(const Vector4& v1, const Vector4& v2, Vector4* dst)
	{
		SC_ASSERT(dst);

		dst->x = v1.x - v2.x;
		dst->y = v1.y - v2.y;
		dst->z = v1.z - v2.z;
		dst->w = v1.w - v2.w;
	}

} // namespace scythe