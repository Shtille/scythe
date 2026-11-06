#include <scythe/core/color.h>

#include <algorithm>
#include <cmath>

namespace scythe {

	constexpr Color::Color(float red, float green, float blue, float alpha)
		: r(red), g(green), b(blue), a(alpha)
	{
	}
	constexpr Color Color::FromBytes(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	{
		return Color(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
	}
	constexpr Color Color::FromCmyk(float c, float m, float y, float k, float alpha)
	{
		// Clamp inputs to guarantee they stay within safety limits (0.0 to 1.0)
		c = std::clamp(c, 0.0f, 1.0f);
		m = std::clamp(m, 0.0f, 1.0f);
		y = std::clamp(y, 0.0f, 1.0f);
		k = std::clamp(k, 0.0f, 1.0f);

		// Compute standard subtractive color space metrics
		float r = (1.0f - c) * (1.0f - k);
		float g = (1.0f - m) * (1.0f - k);
		float b = (1.0f - y) * (1.0f - k);

		return Color{ r, g, b, alpha };
	}
	constexpr Color Color::FromHsv(float h, float s, float v, float alpha)
	{
		h = std::clamp(h, 0.0f, 360.0f);
		s = std::clamp(s, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);

		// If saturation is 0, the color is grayscale
		if (s == 0.0f) {
			return Color{ v, v, v, alpha };
		}

		// Treat 360 degrees the same as 0 degrees
		float sectorAngle = (h >= 360.0f) ? 0.0f : (h / 60.0f);
		int sectorIndex = static_cast<int>(std::floor(sectorAngle));
		float fractionalPart = sectorAngle - sectorIndex;

		// Precompute helper values
		float p = v * (1.0f - s);
		float q = v * (1.0f - (s * fractionalPart));
		float t = v * (1.0f - (s * (1.0f - fractionalPart)));

		switch (sectorIndex) {
			case 0:  return Color{ v, t, p, alpha };
			case 1:  return Color{ q, v, p, alpha };
			case 2:  return Color{ p, v, t, alpha };
			case 3:  return Color{ p, q, v, alpha };
			case 4:  return Color{ t, p, v, alpha };
			case 5:  
			default: return Color{ v, p, q, alpha };
		}
	}
	constexpr Color Color::FromCieXYZ(float x, float y, float z, float alpha)
	{
		// Step 1: Matrix multiplication to get Linear RGB (sRGB D65 Standard)
		float linearR =  3.2406f * x - 1.5372f * y - 0.4986f * z;
		float linearG = -0.9689f * x + 1.8758f * y + 0.0415f * z;
		float linearB =  0.0557f * x - 0.2040f * y + 1.0570f * z;

		// Step 2: Apply standard sRGB Gamma Correction
		auto applyGamma = [](float linearVal) {
			if (linearVal <= 0.0031308f) {
				return linearVal * 12.92f;
			} else {
				return 1.055f * std::pow(linearVal, 1.0f / 2.4f) - 0.055f;
			}
		};

		Color result;
		result.r = std::clamp(applyGamma(linearR), 0.0f, 1.0f);
		result.g = std::clamp(applyGamma(linearG), 0.0f, 1.0f);
		result.b = std::clamp(applyGamma(linearB), 0.0f, 1.0f);
		result.a = alpha;

		return result;
	}
	constexpr Color Color::FromYuv(float y, float u, float v, float alpha)
	{
		// Clamp inputs to safe normalization bounds
		y = std::clamp(y, 0.0f, 1.0f);
		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);

		// Shift Chrominance channels from [0, 1] to the original [-0.5, 0.5] range
		float u_centered = u - 0.5f;
		float v_centered = v - 0.5f;

		// Execute standard ITU-R BT.601 transformation matrix operations
		float r = y + 1.402f * v_centered;
		float g = y - 0.344136f * u_centered - 0.714136f * v_centered;
		float b = y + 1.772f * u_centered;

		return Color{
			std::clamp(r, 0.0f, 1.0f),
			std::clamp(g, 0.0f, 1.0f),
			std::clamp(b, 0.0f, 1.0f),
			alpha
		};
	}

}