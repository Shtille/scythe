#ifndef __SCYTHE_LABEL_H__
#define __SCYTHE_LABEL_H__

#include "widget.h"
#include "graphics/renderer.h"
#include "graphics/text.h"

namespace scythe {

	//! Label class, holds DynamicText object
	class Label : public Widget {
	public:
		Label(Renderer * renderer, Shader * shader,
			  Font * font, const Vector4& color, F32 text_height, U32 buffer_size,
			  F32 x, F32 y, U32 flags);
		virtual ~Label();

		void set_constant_position(bool use);

		void Render() override;
		
		void AppendSymbol(wchar_t symbol);
		void SetText(const wchar_t* text);

		void AlignCenter(F32 rect_width, F32 rect_height);

	protected:
		Shader * shader_;
		Font * font_;
		Vector4 color_;
		DynamicText * text_;
		F32 text_height_;
		bool constant_position_; //!< used for optimization
		
	private:
		void BindConstUniforms();
	};

} // namespace scythe

#endif