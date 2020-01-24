#ifndef __SCYTHE_BUTTON_H__
#define __SCYTHE_BUTTON_H__

#include "rect.h"

namespace scythe {

	//! Standart button class
	class Button : public Rect {
	public:
		Button(F32 x, F32 y, F32 width, F32 height, U32 flags);

		bool is_touched() const;

		void OnTouchDown(const Vector2& position); //!< position is in global coordinate system
		void OnTouchMove(const Vector2& position);
		void OnTouchUp(const Vector2& position);
		
	protected:
		bool is_touched_;
	};
	
	//! Colored button class
	class ButtonColored : public Button, public UiDrawable {
	public:
		ButtonColored(Renderer * renderer, Shader * shader,
			const Vector4& normal_color, const Vector4& touch_color,
			F32 x, F32 y, F32 width, F32 height, U32 flags);
		
		virtual void Render() override;
		
	protected:
		Vector4 normal_color_;
		Vector4 touch_color_;

	private:
		virtual void BindConstUniforms() override;
		virtual void FillVertexAttribs() override;
		virtual void FillVertices() override;
	};

	//! Colored button class
	class ButtonTextured : public Button, public UiDrawable {
	public:
		ButtonTextured(Renderer * renderer, Shader * shader,
			Texture * normal_texture, Texture * touch_texture,
			F32 x, F32 y, F32 width, F32 height, U32 flags);
		
		virtual void Render() override;

	protected:
		Texture * touch_texture_;

	private:
		virtual void BindConstUniforms() override;
		virtual void FillVertexAttribs() override;
		virtual void FillVertices() override;
	};

} // namespace scythe

#endif