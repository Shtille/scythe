#ifndef __SCYTHE_RECT_H__
#define __SCYTHE_RECT_H__

#include "widget.h"
#include "ui_drawable.h"

namespace scythe {

	//! Standart rectangle class
	class Rect : public Widget {
	public:
		Rect(F32 x, F32 y, F32 width, F32 height, U32 flags);

		F32 width() const;
		F32 height() const;

		void set_width(F32 width);
		void set_height(F32 height);

		bool IsInsideLocal(F32 x, F32 y);
		bool IsInsideGlobal(F32 x, F32 y);

	protected:
		F32 width_;
		F32 height_;
	};
	
	//! Colored rectangle class
	class RectColored : public Rect, public UiDrawable {
	public:
		RectColored(Renderer * renderer, Shader * shader, const Vector4& color,
			 F32 x, F32 y, F32 width, F32 height, U32 flags);
		
		virtual void Render() override;
		
	protected:
		Vector4 color_;
		
	private:
		virtual void BindConstUniforms() override;
		virtual void FillVertexAttribs() override;
		virtual void FillVertices() override;
	};
	
	//! Textured rectangle class
	class RectTextured : public Rect, public UiDrawable {
	public:
		RectTextured(Renderer * renderer, Shader * shader,
			Texture * texture, F32 x, F32 y, F32 width, F32 height, U32 flags);
		
		virtual void Render() override;
		
	private:
		virtual void BindConstUniforms() override;
		virtual void FillVertexAttribs() override;
		virtual void FillVertices() override;
	};

} // namespace scythe

#endif