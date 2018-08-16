#ifndef __SCYTHE_LABEL_H__
#define __SCYTHE_LABEL_H__

#include "widget.h"
#include "renderer/renderer.h"
#include "renderer/text.h"

namespace scythe {

	//! Label class, holds DynamicText object
	class Label : public Widget {
	public:
		Label(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
			  sht::graphics::Font * font, const vec4& color, F32 text_height, U32 buffer_size,
			  F32 x, F32 y, U32 flags);
		virtual ~Label();

		void set_constant_position(bool use);

		void Render() override;
		
		void AppendSymbol(wchar_t symbol);
		void SetText(const wchar_t* text);

		void AlignCenter(F32 rect_width, F32 rect_height);

	protected:
		sht::graphics::Shader * shader_;
		sht::graphics::Font * font_;
		vec4 color_;
		sht::graphics::DynamicText * text_;
		F32 text_height_;
		bool constant_position_; //!< used for optimization
		
	private:
		void BindConstUniforms();
	};

} // namespace scythe

#endif