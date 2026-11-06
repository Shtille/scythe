#ifndef __SCYTHE_COLOR_H__
#define __SCYTHE_COLOR_H__

#include "../types.h"

namespace scythe {

	/**
	 * Defines a color structure.
	 */
	struct alignas(16) Color
	{
		// Default constructor (Black, fully opaque)
		Color() = default;

		// Direct normalized float input (0.0f to 1.0f)
		constexpr Color(float red, float green, float blue, float alpha = 1.0f);

		// Convenience constructor for standard 0-255 integer bytes
		static constexpr Color FromBytes(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);

		// Converts CMYK normalized floats (0.0f to 1.0f) to an RGBA Color struct
		static constexpr Color FromCmyk(float c, float m, float y, float k, float alpha = 1.0f);

		// Converts HSV to RGBA (H: 0.0f to 360.0f, S/V/Alpha: 0.0f to 1.0f)
		static constexpr Color FromHsv(float h, float s, float v, float alpha = 1.0f);

		// Converts CIE 1931 XYZ to RGBA (Inputs typically range from 0.0 to 1.0+)
		static constexpr Color FromCieXYZ(float x, float y, float z, float alpha = 1.0f);

		// Converts digital YUV (BT.601) to RGBA. All inputs range from 0.0f to 1.0f.
		static constexpr Color FromYuv(float y, float u, float v, float alpha = 1.0f);

		// Access raw memory address safely for OpenGL pointers
		const float* data() const { return &r; }

		F32 r = 0.0f;
		F32 g = 0.0f;
		F32 b = 0.0f;
		F32 a = 1.0f;
	};

} // namespace scythe

#endif