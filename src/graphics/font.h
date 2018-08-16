#ifndef __SCYTHE_FONT_H__
#define __SCYTHE_FONT_H__

#include "common/types.h"
#include "resource.h"

#include <unordered_map>

namespace scythe {
		
	class Image;
	class Texture;
	
	//! Struct containing all offsets data
	struct FontCharInfo {
		float advance_x;
		float advance_y;
		float bitmap_width;
		float bitmap_height;
		float bitmap_left;
		float bitmap_top;
		float texcoord_x;
		float texcoord_y;
	};
	
	struct FontGlyphPoint {
		float position_x;
		float position_y;
		float texcoord_x;
		float texcoord_y;
	};
	
	//! Font class
	class Font : public Resource {
		friend class Renderer;
		friend class OpenGlRenderer;
		
	public:
		const FontCharInfo* info(U32 charcode) const;
		const int atlas_width() const;
		const int atlas_height() const;
		Texture * texture() const;
		const float font_height() const;
		
		bool MakeAtlas(const char* filename, int font_height, Image * image);
		
	protected:
		Font();
		~Font();
		Font(const Font&) = delete;
		void operator = (const Font&) = delete;
		
	private:
		Texture * texture_; // pointer to a textore, don't need to delete (or use shared pointers)
		std::unordered_map<U32, FontCharInfo> info_map_;
		float font_height_; //!< for atlas
	};

} // namespace scythe

#endif