#ifndef __SCYTHE_TEXTURE_H__
#define __SCYTHE_TEXTURE_H__

#include "common/types.h"
#include "resource.h"
#include "image/image.h"

namespace scythe {

	//! Texture class
	class Texture : public Resource {
		friend class Font;
		friend class Renderer;
		friend class OpenGlRenderer;

	public:

		//! Texture filtering types
		enum class Filter {
			kPoint,
			kLinear,
			kBilinear,
			kTrilinear,
			kBilinearAniso,
			kTrilinearAniso
		};

		//! Texture wrapping types
		enum class Wrap {
			kClamp,
			kClampToEdge,
			kRepeat
		};

		virtual U32 GetSrcFormat() = 0;
		virtual U32 GetSrcType() = 0;
		virtual int GetInternalFormat() = 0;

		virtual void SetFilter(Filter filter) = 0;
		virtual void SetWrap(Wrap wrap) = 0;

		int width() const;
		int height() const;

		U32 GetSize();			//!< size of image in memory (w*h*bpp)

	protected:
		Texture();
		virtual ~Texture();
		Texture(const Texture&) = delete;
		void operator = (const Texture&) = delete;

		virtual void ChooseTarget() = 0;

		U32 target_;			//!< texture target
		U32 texture_id_;		//!< texture ID
		U32 depth_id_;			//!< depth id
		U32 stencil_id_;		//!< stencil id
		Image::Format format_;	//!< image format type
		int width_;				//!< width of texture
		int height_;			//!< height of texture
	};

} // namespace scythe

#endif