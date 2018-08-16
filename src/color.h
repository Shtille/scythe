#ifndef __SCYTHE_COLOR_H__
#define __SCYTHE_COLOR_H__

#include "common/types.h"

namespace scythe {
		
	// Forward declarations
	class RgbColor;
	class RgbaColor;
	class PackedRgbColor;
	class PackedRgbaColor;
	
	//! Single precision floating-point component RGB storage
	class RgbColor {
		friend class RgbaColor;
		
	public:
		RgbColor() = default;
		RgbColor(const RgbColor& rgb_color);
		RgbColor(const RgbaColor& rgba_color);
		RgbColor(F32 red, F32 green, F32 blue);
		
		F32 red() const;
		F32 green() const;
		F32 blue() const;
		
		const float* AsArray() const;
		
		static RgbColor Mix(const RgbColor& color1, const RgbColor& color2, F32 t);
		
	private:
		F32 red_;
		F32 green_;
		F32 blue_;
	};
	
	//! Single precision floating-point component RGBA storage
	class RgbaColor {
		friend class RgbColor;
		
	public:
		RgbaColor() = default;
		RgbaColor(const RgbaColor& rgba_color);
		RgbaColor(const RgbColor& rgb_color, F32 alpha);
		RgbaColor(F32 red, F32 green, F32 blue, F32 alpha);
		
		F32 red() const;
		F32 green() const;
		F32 blue() const;
		F32 alpha() const;
		
		const float* AsArray() const;
		
		static RgbaColor Mix(const RgbaColor& color1, const RgbaColor& color2, F32 t);
		
	private:
		F32 red_;
		F32 green_;
		F32 blue_;
		F32 alpha_;
	};
	
	//! An RGB color packed into a 32 bit value, alpha byte is always 0xFF
	class PackedRgbColor {
		friend class PackedRgbaColor;
		
	public:
		PackedRgbColor() = default;
		PackedRgbColor(const PackedRgbColor& rgb_color);
		PackedRgbColor(const PackedRgbaColor& rgba_color);
		PackedRgbColor(const RgbColor& rgb_color);
		PackedRgbColor(const RgbaColor& rgba_color);
		
		F32 red() const;
		F32 green() const;
		F32 blue() const;
		
		RgbColor Unpack();
		
		static PackedRgbColor Mix(const PackedRgbColor& color1, const PackedRgbColor& color2, F32 t);
		
	private:
		U32 data_;
	};
	
	//! An RGBA color packed into a 32 bit value
	class PackedRgbaColor {
		friend class PackedRgbColor;
		
	public:
		PackedRgbaColor() = default;
		PackedRgbaColor(const PackedRgbaColor& rgba_color);
		PackedRgbaColor(const PackedRgbColor& rgb_color);
		PackedRgbaColor(const RgbColor& rgb_color);
		PackedRgbaColor(const RgbaColor& rgba_color);
		
		F32 red() const;
		F32 green() const;
		F32 blue() const;
		F32 alpha() const;
		
		RgbaColor Unpack();
		
		static PackedRgbaColor Mix(const PackedRgbaColor& color1, const PackedRgbaColor& color2, F32 t);
		
	private:
		U32 data_;
	};
		
} // namespace scythe

#endif