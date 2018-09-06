#ifndef __SCYTHE_SLIDER_H__
#define __SCYTHE_SLIDER_H__

#include "widget.h"
#include "drawable.h"

namespace scythe {

	/*! Standart slider class
	Radius of slider sides is equal to the half of height/width.
	*/
	class Slider : public Widget {
	public:
		Slider(F32 x, F32 y, F32 width, F32 height, F32 bar_radius, U32 flags);

		void SetPinPosition(F32 pos);

		void OnTouchDown(const vec2& position); //!< position is in global coordinate system
		void OnTouchMove(const vec2& position);
		void OnTouchUp(const vec2& position);

		F32 width() const;
		F32 height() const;
		float pin_position() const;
		float pin_radius() const;
		bool is_vertical() const;
		bool is_touched() const;

	protected:
		bool IsInsidePin(const vec2& global_position);

		F32 width_;
		F32 height_;
		vec2 old_position_;
		float radius_;
		float bar_radius_;
		float pin_position_;
		float pin_radius_;
		bool is_touched_;
	};

	//! Colored slider class
	class SliderColored : public Slider, public Drawable {
	public:
		enum Form
		{
			kQuad,
			kCircle
		};
		SliderColored(Renderer * renderer, Shader * shader,
			const vec4& bar_color, const vec4& pin_color_normal, const vec4& pin_color_touch,
			F32 x, F32 y, F32 width, F32 height, F32 bar_radius, U32 flags,
			Form bar_form = kQuad, Form pin_form = kQuad);

		virtual void Render() override;

	protected:
		vec4 bar_color_;
		vec4 pin_color_normal_;
		vec4 pin_color_touch_;
		U32 num_bar_vertices_;
		U32 num_pin_vertices_;
		Form bar_form_;
		Form pin_form_;

	private:
		void BindConstUniforms() override;
		void FillVertexAttribs() override;
		void FillVertices() override;
	};

	//! Textured slider class
	class SliderTextured : public Slider, public Drawable {
	public:
		enum Form
		{
			kQuad,
			kCircle
		};
		SliderTextured(Renderer * renderer,
			Shader * color_shader, Shader * texture_shader,
			Texture * texture_normal, Texture * texture_touch,
			const vec4& bar_color, F32 x, F32 y, F32 width, F32 height, F32 bar_radius, U32 flags,
			Form bar_form = kQuad);

		virtual void Render() override;

	protected:
		Shader * texture_shader_;
		Texture * texture_touch_;
		vec4 bar_color_;
		U32 num_bar_vertices_;
		U32 num_pin_vertices_;
		Form bar_form_;

	private:
		void BindConstUniforms() override;
		void FillVertexAttribs() override;
		void FillVertices() override;
	};

} // namespace scythe

#endif