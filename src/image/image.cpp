#include "image.h"

#include "filesystem/filename.h"
#include "memory/allocator.h"
#include "common/sc_assert.h"

#include <assert.h>
#include <math.h>
#include <memory.h>

namespace scythe {

	static int GetBpp(Image::Format fmt)
		// Number of bits per pixel
	{
		switch (fmt)
		{
		case Image::Format::kA8:
		case Image::Format::kI8:
		case Image::Format::kL8:
		case Image::Format::kR8:
			return 8;
		case Image::Format::kA16:
		case Image::Format::kI16:
		case Image::Format::kL16:
		case Image::Format::kR16:
			return 16;
		case Image::Format::kA32:
		case Image::Format::kI32:
		case Image::Format::kL32:
		case Image::Format::kR32:
			return 32;
		case Image::Format::kLA8:
		case Image::Format::kRG8:
			return 16;
		case Image::Format::kLA16:
		case Image::Format::kRG16:
			return 32;
		case Image::Format::kLA32:
		case Image::Format::kRG32:
			return 64;
		case Image::Format::kRGB8:
			return 24;
		case Image::Format::kRGB16:
			return 48;
		case Image::Format::kRGB32:
			return 96;
		case Image::Format::kRGBA8:
			return 32;
		case Image::Format::kRGBA16:
			return 64;
		case Image::Format::kRGBA32:
			return 128;
		default:
			assert(false && "unknown image format");
			return 24;
		}
	}
	static int GetChannels(Image::Format fmt)
	// Number of channels
	{
		switch (fmt)
		{
			case Image::Format::kA8:
			case Image::Format::kI8:
			case Image::Format::kL8:
			case Image::Format::kR8:
			case Image::Format::kA16:
			case Image::Format::kI16:
			case Image::Format::kL16:
			case Image::Format::kR16:
			case Image::Format::kA32:
			case Image::Format::kI32:
			case Image::Format::kL32:
			case Image::Format::kR32:
				return 1;
			case Image::Format::kLA8:
			case Image::Format::kRG8:
			case Image::Format::kLA16:
			case Image::Format::kRG16:
			case Image::Format::kLA32:
			case Image::Format::kRG32:
				return 2;
			case Image::Format::kRGB8:
			case Image::Format::kRGB16:
			case Image::Format::kRGB32:
				return 3;
			case Image::Format::kRGBA8:
			case Image::Format::kRGBA16:
			case Image::Format::kRGBA32:
				return 4;
			default:
				assert(false && "unknown image format");
				return 3;
		}
	}
	static Image::FileFormat ExtractFileFormat(const char* filename)
	{
		Filename fn(filename);
		std::string ext = fn.ExtractExt();
		if (ext == "bmp")
			return Image::FileFormat::kBmp;
		else if (ext == "jpg")
			return Image::FileFormat::kJpg;
		else if (ext == "png")
			return Image::FileFormat::kPng;
		else if (ext == "tga")
			return Image::FileFormat::kTga;
		else if (ext == "tif" || ext == "tiff")
			return Image::FileFormat::kTif;
		else if (ext == "hdr")
			return Image::FileFormat::kHdr;
		return Image::FileFormat::kUnknown;
	}
	Image::Image()
	: allocator_(nullptr)
	, pixels_(nullptr)
	, inverted_row_order_(true)
	{
	}
	Image(Allocator * allocator)
	: allocator_(allocator)
	, pixels_(nullptr)
	, inverted_row_order_(true)
	{
	}
	Image::Image(const Image& other)
	: allocator_(other.allocator_)
	, pixels_(nullptr)
	, format_(other.format_)
	, data_type_(other.data_type_)
	, width_(other.width_)
	, height_(other.height_)
	, channels_(other.channels_)
	, bpp_(other.bpp_)
	, inverted_row_order_(other.inverted_row_order_)
	{
		// Copy pixels
		size_t size = static_cast<size_t>(width_ * height_ * bpp_);
		AllocatePixels(size);
		memcpy(pixels_, other.pixels_, size);
	}
	Image::~Image()
	{
		FreePixels();
	}
	void Image::AllocatePixels(const size_t allocation_size)
	{
		if (allocator_ != nullptr)
			pixels_ = reinterpret_cast<U8*>(allocator_->Allocate(allocation_size));
		else
			pixels_ = new U8[allocation_size];
	}
	void Image::FreePixels()
	{
		if (allocator_ != nullptr)
			allocator_->Free(reinterpret_cast<void*>(pixels_));
		else if (pixels_)
		{
			delete[] pixels_;
		}
		pixels_ = nullptr;
	}
	void Image::ReallocatePixels(const size_t allocation_size)
	{
		FreePixels();
		AllocatePixels(allocation_size);
	}
	void Image::SetRowOrder(bool inverted)
	{
		inverted_row_order_ = inverted;
	}
	U8* Image::pixels()
	{
		return pixels_;
	}
	const U8* Image::pixels() const
	{
		return pixels_;
	}
	Image::Format Image::format() const
	{
		return format_;
	}
	int Image::width() const
	{
		return width_;
	}
	int Image::height() const
	{
		return height_;
	}
	int Image::bpp() const
	{
		return bpp_;
	}
	void Image::SwapRedBlueChannels()
	{
		int unit_size = bpp_ / channels_;
		U8 *temp = new U8[unit_size];
		for (int i = 0; i < width_*height_*bpp_; i += bpp_)
		{
			memcpy(temp, &pixels_[i], unit_size);							// store B
			memcpy(&pixels_[i], &pixels_[i + 2 * unit_size], unit_size);	// R to B
			memcpy(&pixels_[i + 2 * unit_size], temp, unit_size);			// restore B
		}
		delete[] temp;
	}
	U8* Image::Allocate(int w, int h, Format fmt)
	{
		width_ = w;
		height_ = h;
		format_ = fmt;
		int bpp = GetBpp(fmt);
		bpp_ = bpp >> 3; // bits to bytes
		channels_ = GetChannels(fmt);
		ReallocatePixels(width_ * height_ * bpp_);
		return pixels_;
	}
	void Image::FillWithZeroes()
	{
		memset(pixels_, 0, width_ * height_ * bpp_);
	}
	void Image::Copy(const Image& other)
	{
		size_t size = static_cast<size_t>(width_ * height_ * bpp_);
		ReallocatePixels(size);
		memcpy(pixels_, other.pixels_, size);
		format_ = other.format_;
		data_type_ = other.data_type_;
		width_ = other.width_;
		height_ = other.height_;
		channels_ = other.channels_;
		bpp_ = other.bpp_;
		inverted_row_order_ = other.inverted_row_order_;
	}
	void Image::CopyData(int offset_x, int offset_y, int source_width, const U8* data)
	{
		// Data should have the same bpp as source
		for (int y = 0; y < height_; ++y)
		{
			const U8* src = &data[((y + offset_y)*source_width + offset_x)*bpp_];
			U8* dst = &pixels_[(y*width_)*bpp_];
			memcpy(dst, src, width_ * bpp_);
		}
	}
	void Image::SubData(int offset_x, int offset_y, int w, int h, const U8* data)
	{
		int max_x = offset_x + w;
		int max_y = offset_y + h;
		assert(max_x <= width_);
		assert(max_y <= height_);
		
		// Data should have the same bpp as source
		for (int y = 0; y < h; ++y)
		{
			U8* dst = &pixels_[((y + offset_y)*width_ + offset_x)*bpp_];
			const U8* src = &data[(y*w)*bpp_];
			memcpy(dst, src, w * bpp_);
		}
	}
	void Image::SubDataColored(int offset_x, int offset_y, int w, int h, const U8* data,
		const RgbColor& color)
	{
		int max_x = offset_x + w;
		int max_y = offset_y + h;
		assert(max_x <= width_);
		assert(max_y <= height_);
		assert(bpp_ == 4);

		U8 r = static_cast<U8>(color.red()   * 255.0f);
		U8 g = static_cast<U8>(color.green() * 255.0f);
		U8 b = static_cast<U8>(color.blue()  * 255.0f);

		const U8* src = data;

		// Destination should have 4 bytes per pixel while source - only 1
		for (int y = offset_y; y < max_y; ++y)
		{
			for (int x = offset_x; x < max_x; ++x)
			{
				U8* dst = &pixels_[(y*width_ + x)*bpp_];
				*dst++ = r;
				*dst++ = g;
				*dst++ = b;
				*dst = *src++;
			}
		}
	}
	void Image::SubDataAlphaBlend(int offset_x, int offset_y, int w, int h, const U8* data,
		const RgbColor& color)
	{
		int max_x = offset_x + w;
		int max_y = offset_y + h;
		assert(max_x <= width_);
		assert(max_y <= height_);
		assert(bpp_ == 4);

		const U8* src = data;

		// Destination should have 4 bytes per pixel while source - only 1
		for (int y = offset_y; y < max_y; ++y)
		{
			for (int x = offset_x; x < max_x; ++x)
			{
				U8* dst = &pixels_[(y*width_ + x)*bpp_];

				float base_r = (float)(*(dst  ))/255.0f;
				float base_g = (float)(*(dst+1))/255.0f;
				float base_b = (float)(*(dst+2))/255.0f;
				float base_a = (float)(*(dst+3))/255.0f;
				float blend_r = color.red();
				float blend_g = color.green();
				float blend_b = color.blue();
				float blend_a = (float)(*src)/255.0f;
				float out_r = (1.0f - blend_a) * base_r + blend_a * blend_r;
				float out_g = (1.0f - blend_a) * base_g + blend_a * blend_g;
				float out_b = (1.0f - blend_a) * base_b + blend_a * blend_b;
				float out_a = (1.0f - blend_a) * base_a + blend_a * blend_a;
				*dst++ = (U8)(out_r * 255.0f);
				*dst++ = (U8)(out_g * 255.0f);
				*dst++ = (U8)(out_b * 255.0f);
				*dst   = (U8)(out_a * 255.0f);
				++src;
			}
		}
	}
	bool Image::Save(const char* filename)
	{
		FileFormat fmt = ExtractFileFormat(filename);
		switch (fmt)
		{
		case Image::FileFormat::kBmp:
			return SaveBmp(filename);
		case Image::FileFormat::kJpg:
			return SaveJpeg(filename);
		case Image::FileFormat::kPng:
			return SavePng(filename);
		case Image::FileFormat::kTga:
			return SaveTga(filename);
		case Image::FileFormat::kTif:
			return SaveTiff(filename);
		case Image::FileFormat::kHdr:
			return SaveHdr(filename);
		default:
			assert(!"unknown image format");
			return false;
		}
	}
	bool Image::LoadFromFile(const char* filename)
	{
		FileFormat fmt = ExtractFileFormat(filename);
		switch (fmt)
		{
		case Image::FileFormat::kBmp:
			return LoadBmp(filename);
		case Image::FileFormat::kJpg:
			return LoadJpeg(filename);
		case Image::FileFormat::kPng:
			return LoadPng(filename);
		case Image::FileFormat::kTga:
			return LoadTga(filename);
		case Image::FileFormat::kTif:
			return LoadTiff(filename);
		case Image::FileFormat::kHdr:
			return LoadHdr(filename);
		default:
			assert(!"unknown image format");
			return false;
		}
	}
	bool Image::LoadFromBuffer(const U8* buffer, size_t length)
	{
		// TODO: We need to recognize image type from buffer
		assert(false);
		FileFormat fmt = Image::FileFormat::kJpg;
		switch (fmt)
		{
		case Image::FileFormat::kBmp:
			return LoadFromBufferBmp(buffer, length);
		case Image::FileFormat::kJpg:
			return LoadFromBufferJpeg(buffer, length);
		case Image::FileFormat::kPng:
			return LoadFromBufferPng(buffer, length);
		case Image::FileFormat::kTga:
			return LoadFromBufferTga(buffer, length);
		case Image::FileFormat::kTif:
			return LoadFromBufferTiff(buffer, length);
		case Image::FileFormat::kHdr:
			return LoadFromBufferHdr(buffer, length);
		default:
			assert(!"unknown image format");
			return false;
		}
	}
	bool Image::LoadNMapFromHMap(const char* filename)
	{
		if (!LoadFromFile(filename))
			return false;

		SC_ASSERT(allocator_ == nullptr);

		int new_bpp = 3;
		U8 * new_pixels = new U8[width_ * height_ * new_bpp];

		if (data_type_ == DataType::kUint8) // standart bitmap
		{
			const float kOneOver255 = 1.0f / 255.0f;
							int offs = 0;
			for (int i = 0; i < height_; i++)
				for (int j = 0; j < width_; j++)
				{
					float c  = (float)pixels_[(i*width_ + j)*channels_];
					float cx = (float)pixels_[(i*width_ + (j + 1) % width_)*channels_];
					float cy = (float)pixels_[(((i + 1) % height_)*width_ + j)*channels_];
					// convert height values to [0,1] range
					c  *= kOneOver255;
					cx *= kOneOver255;
					cy *= kOneOver255;

					// find derivatives
					float k = 3.0f;
					float dx = (c - cx) * k;
					float dy = (c - cy) * k;

					// normalize
					float len = sqrt(dx*dx + dy*dy + 1);

					// get normal
					float nx = dy / len;
					float ny = -dx / len;
					float nz = 1.0f / len;

					new_pixels[offs    ] = (U8)(128 + 127 * nx);
					new_pixels[offs + 1] = (U8)(128 + 127 * ny);
					new_pixels[offs + 2] = (U8)(128 + 127 * nz);
					offs += 3;
				}
		}
		else if (data_type_ == DataType::kFloat)
		{
							int offs = 0;
			for (int i = 0; i < height_; i++)
				for (int j = 0; j < width_; j++)
				{
					float c  = reinterpret_cast<float*>(pixels_)[(i*width_ + j)*channels_];
					float cx = reinterpret_cast<float*>(pixels_)[(i*width_ + (j + 1) % width_)*channels_];
					float cy = reinterpret_cast<float*>(pixels_)[(((i + 1) % height_)*width_ + j)*channels_];

					// find derivatives
					float k = 3.0f;
					float dx = (c - cx) * k;
					float dy = (c - cy) * k;

					// normalize
					float len = sqrt(dx*dx + dy*dy + 1);

					// get normal
					float nx = dy / len;
					float ny = -dx / len;
					float nz = 1.0f / len;

					reinterpret_cast<float*>(new_pixels)[offs    ] = nx;
					reinterpret_cast<float*>(new_pixels)[offs + 1] = ny;
					reinterpret_cast<float*>(new_pixels)[offs + 2] = nz;
					offs += 3;
				}
		}
		else // assume its floating point format
		{
			assert(!"unknown data type");
			return false;
		}

		delete[] pixels_;
		pixels_ = new_pixels;
		channels_ = 3;
		data_type_ = DataType::kUint8;
		format_ = Format::kRGB8;

		return true;
	}
	bool Image::LoadNHMapFromHMap(const char* filename)
	{
		if (!LoadFromFile(filename))
			return false;

		SC_ASSERT(allocator_ == nullptr);

		int new_bpp = 4;
		U8 * new_pixels = new U8[width_ * height_ * new_bpp];

		if (data_type_ == DataType::kUint8) // standart bitmap
		{
			const float kOneOver255 = 1.0f / 255.0f;
							int offs = 0;
			for (int i = 0; i < height_; i++)
				for (int j = 0; j < width_; j++)
				{
					float c  = (float)pixels_[(i*width_ + j)*channels_];
					float cx = (float)pixels_[(i*width_ + (j + 1) % width_)*channels_];
					float cy = (float)pixels_[(((i + 1) % height_)*width_ + j)*channels_];
					// convert height values to [0,1] range
					c  *= kOneOver255;
					cx *= kOneOver255;
					cy *= kOneOver255;

					// find derivatives
					float k = 3.0f;
					float dx = (c - cx) * k;
					float dy = (c - cy) * k;

					// normalize
					float len = sqrt(dx*dx + dy*dy + 1);

					// get normal
					float nx = dy / len;
					float ny = -dx / len;
					float nz = 1.0f / len;

					new_pixels[offs    ] = (unsigned char)(128 + 127 * nx);
					new_pixels[offs + 1] = (unsigned char)(128 + 127 * ny);
					new_pixels[offs + 2] = (unsigned char)(128 + 127 * nz);
					new_pixels[offs + 3] = (unsigned char)(255 * c);
					offs += 4;
				}
		}
		else if (data_type_ == DataType::kFloat)
		{
							int offs = 0;
			for (int i = 0; i < height_; i++)
				for (int j = 0; j < width_; j++)
				{
					float c  = reinterpret_cast<float*>(pixels_)[(i*width_ + j)*channels_];
					float cx = reinterpret_cast<float*>(pixels_)[(i*width_ + (j + 1) % width_)*channels_];
					float cy = reinterpret_cast<float*>(pixels_)[(((i + 1) % height_)*width_ + j)*channels_];

					// find derivatives
					float k = 3.0f;
					float dx = (c - cx) * k;
					float dy = (c - cy) * k;

					// normalize
					float len = sqrt(dx*dx + dy*dy + 1);

					// get normal
					float nx = dy / len;
					float ny = -dx / len;
					float nz = 1.0f / len;

					reinterpret_cast<float*>(new_pixels)[offs    ] = nx;
					reinterpret_cast<float*>(new_pixels)[offs + 1] = ny;
					reinterpret_cast<float*>(new_pixels)[offs + 2] = nz;
					reinterpret_cast<float*>(new_pixels)[offs + 3] = c;
					offs += 4;
				}
		}
		else // assume its floating point format
		{
			assert(!"unknown data type");
			return false;
		}

		delete[] pixels_;
		pixels_ = new_pixels;
		channels_ = 4;
		data_type_ = DataType::kUint8;
		format_ = Format::kRGBA8;

		return true;
	}

} // namespace scythe