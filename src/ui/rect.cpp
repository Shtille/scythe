#include "rect.h"

namespace scythe {

	Rect::Rect(F32 x, F32 y, F32 width, F32 height, U32 flags)
	: Widget(x, y, flags)
	, width_(width)
	, height_(height)
	{
	}
	F32 Rect::width() const
	{
		return width_;
	}
	F32 Rect::height() const
	{
		return height_;
	}
	void Rect::set_width(F32 width)
	{
		width_ = width;
	}
	void Rect::set_height(F32 height)
	{
		height_ = height;
	}
	bool Rect::IsInsideLocal(F32 x, F32 y)
	{
		return (position_.x < x) && (position_.y < y) &&
			(x < position_.x + width_) && (y < position_.y + height_);
	}
	bool Rect::IsInsideGlobal(F32 x, F32 y)
	{
		Vector2 position;
		ObtainGlobalPosition(&position);
		return (position.x < x) && (position.y < y) &&
			(x < position.x + width_) && (y < position.y + height_);
	}
	RectColored::RectColored(Renderer * renderer, Shader * shader,
							 const Vector4& color, F32 x, F32 y, F32 width, F32 height, U32 flags)
	: Rect(x, y, width, height, flags)
	, UiDrawable(renderer, shader, nullptr)
	, color_(color)
	{
		FillVertexAttribs();
		FillVertices();
		MakeRenderable();
		BindConstUniforms();
	}
	void RectColored::Render()
	{
		Vector2 position;
		ObtainGlobalPosition(&position);
		shader_->Bind();
		shader_->Uniform2fv("u_position", position);
		shader_->Uniform4fv("u_color", color_);
		UiDrawable::Render();
		shader_->Unbind();
	}
	void RectColored::BindConstUniforms()
	{
		shader_->Bind();
		shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
		shader_->Unbind();
	}
	void RectColored::FillVertexAttribs()
	{
		VertexAttribute attrib(VertexAttribute::kVertex, 2);
		attribs_.push_back(attrib);
	}
	void RectColored::FillVertices()
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
	RectTextured::RectTextured(Renderer * renderer, Shader * shader,
			Texture * texture, F32 x, F32 y, F32 width, F32 height, U32 flags)
	: Rect(x, y, width, height, flags)
	, UiDrawable(renderer, shader, texture)
	{
		FillVertexAttribs();
		FillVertices();
		MakeRenderable();
		BindConstUniforms();
	}
	void RectTextured::Render()
	{
		Vector2 position;
		ObtainGlobalPosition(&position);
		shader_->Bind();
		shader_->Uniform2fv("u_position", position);
		UiDrawable::Render();
		shader_->Unbind();
	}
	void RectTextured::BindConstUniforms()
	{
		shader_->Bind();
		shader_->Uniform1f("u_texture", 0);
		shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
		shader_->Unbind();
	}
	void RectTextured::FillVertexAttribs()
	{
		VertexAttribute attrib(VertexAttribute::kVertex, 4);
		attribs_.push_back(attrib);
	}
	void RectTextured::FillVertices()
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