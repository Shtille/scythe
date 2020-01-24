#include "button.h"

namespace scythe {

	Button::Button(F32 x, F32 y, F32 width, F32 height, U32 flags)
	: Rect(x, y, width, height, flags)
	, is_touched_(false)
	{

	}
	bool Button::is_touched() const
	{
		return is_touched_;
	}
	void Button::OnTouchDown(const Vector2& position)
	{
		if (IsInsideGlobal(position.x, position.y))
		{
			is_touched_ = true;
		}
	}
	void Button::OnTouchMove(const Vector2& position)
	{
		if (is_touched_)
		{
			// If gesture has gone away from the pin disable the pin
			if (!IsInsideGlobal(position.x, position.y))
			{
				is_touched_ = false;
				return;
			}
		}
	}
	void Button::OnTouchUp(const Vector2& position)
	{
		is_touched_ = false;
	}

	ButtonColored::ButtonColored(Renderer * renderer, Shader * shader,
		const Vector4& normal_color, const Vector4& touch_color,
		F32 x, F32 y, F32 width, F32 height, U32 flags)
	: Button(x, y, width, height, flags)
	, UiDrawable(renderer, shader, nullptr)
	, normal_color_(normal_color)
	, touch_color_(touch_color)
	{
		FillVertexAttribs();
		FillVertices();
		MakeRenderable();
		BindConstUniforms();
	}
	void ButtonColored::Render()
	{
		Vector2 position;
		ObtainGlobalPosition(&position);
		shader_->Bind();
		shader_->Uniform2fv("u_position", position);
		shader_->Uniform4fv("u_color", (is_touched_) ? touch_color_ : normal_color_);
		UiDrawable::Render();
		shader_->Unbind();
	}
	void ButtonColored::BindConstUniforms()
	{
		shader_->Bind();
		shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
		shader_->Unbind();
	}
	void ButtonColored::FillVertexAttribs()
	{
		VertexAttribute attrib(VertexAttribute::kVertex, 2);
		attribs_.push_back(attrib);
	}
	void ButtonColored::FillVertices()
	{
		num_vertices_ = 4;
		vertices_array_ = new U8[num_vertices_ * sizeof(Vector2)]; // 4 * 2 * s(float)
		Vector2 * vertices = reinterpret_cast<Vector2*>(vertices_array_);
		
		// Lower left
		vertices[0].x = 0.0f;
		vertices[0].y = 0.0f;
		
		// Lower right
		vertices[1].x = width_;
		vertices[1].y = 0.0f;
		
		// Upper left
		vertices[2].x = 0.0f;
		vertices[2].y = height_;
		
		// Upper right
		vertices[3].x = width_;
		vertices[3].y = height_;
	}
	ButtonTextured::ButtonTextured(Renderer * renderer, Shader * shader,
			Texture * normal_texture, Texture * touch_texture,
			F32 x, F32 y, F32 width, F32 height, U32 flags)
	: Button(x, y, width, height, flags)
	, UiDrawable(renderer, shader, normal_texture)
	, touch_texture_(touch_texture)
	{
		FillVertexAttribs();
		FillVertices();
		MakeRenderable();
		BindConstUniforms();
	}
	void ButtonTextured::Render()
	{
		Vector2 position;
		ObtainGlobalPosition(&position);
		shader_->Bind();
		shader_->Uniform2fv("u_position", position);
		renderer_->ChangeTexture((is_touched_) ? touch_texture_ : texture_);
		renderer_->context()->BindVertexArrayObject(vertex_array_object_);
		renderer_->context()->DrawArrays(PrimitiveType::kTriangleStrip, 0, num_vertices_);
		renderer_->context()->BindVertexArrayObject(0);
		shader_->Unbind();
	}
	void ButtonTextured::BindConstUniforms()
	{
		shader_->Bind();
		shader_->Uniform1f("u_texture", 0);
		shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
		shader_->Unbind();
	}
	void ButtonTextured::FillVertexAttribs()
	{
		VertexAttribute attrib(VertexAttribute::kVertex, 4);
		attribs_.push_back(attrib);
	}
	void ButtonTextured::FillVertices()
	{
		num_vertices_ = 4;
		vertices_array_ = new U8[num_vertices_ * sizeof(Vector4)]; // x, y, tx, ty
		Vector4 * vertices = reinterpret_cast<Vector4*>(vertices_array_);
		
		// Lower left
		vertices[0].x = 0.0f;
		vertices[0].y = 0.0f;
		vertices[0].z = 0.0f;
		vertices[0].w = 0.0f;
		
		// Lower right
		vertices[1].x = width_;
		vertices[1].y = 0.0f;
		vertices[1].z = 1.0f;
		vertices[1].w = 0.0f;
		
		// Upper left
		vertices[2].x = 0.0f;
		vertices[2].y = height_;
		vertices[2].z = 0.0f;
		vertices[2].w = 1.0f;
		
		// Upper right
		vertices[3].x = width_;
		vertices[3].y = height_;
		vertices[3].z = 1.0f;
		vertices[3].w = 1.0f;
	}

} // namespace scythe