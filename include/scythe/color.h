#ifndef __SCYTHE_COLOR_H__
#define __SCYTHE_COLOR_H__

#include "types.h"

namespace scythe {

	struct alignas(sizeof(F32) * 4) Color
	{
		F32 r = 0.0f;
		F32 g = 0.0f;
		F32 b = 0.0f;
		F32 a = 1.0f;

		// Default constructor (Black, fully opaque)
		Color() = default;

		// Direct normalized float input (0.0f to 1.0f)
		constexpr Color(float red, float green, float blue, float alpha = 1.0f)
			: r(red), g(green), b(blue), a(alpha) {}

		// Convenience constructor for standard 0-255 integer bytes
		static constexpr Color fromBytes(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) {
			return Color(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
		}

		// Access raw memory address safely for OpenGL pointers
		const float* data() const { return &r; }
	};

} // namespace scythe

#endif