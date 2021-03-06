#include "ui_drawable.h"

namespace scythe {

	UiDrawable::UiDrawable(Renderer * renderer, Shader * shader, Texture * texture)
	: renderer_(renderer)
	, shader_(shader)
	, texture_(texture)
	, vertex_format_(nullptr)
	, vertex_buffer_(nullptr)
	, vertex_array_object_(0)
	, num_vertices_(0)
	, vertices_array_(nullptr)
	, can_render_(false)
	{

	}
	UiDrawable::~UiDrawable()
	{
		if (vertex_format_)
			renderer_->DeleteVertexFormat(vertex_format_);
		if (vertex_buffer_)
			renderer_->DeleteVertexBuffer(vertex_buffer_);
		if (vertex_array_object_)
			renderer_->context()->DeleteVertexArrayObject(vertex_array_object_);
		FreeArrays();
	}
	void UiDrawable::FreeArrays()
	{
		if (vertices_array_)
		{
			delete[] vertices_array_;
			vertices_array_ = nullptr;
		}
	}
	void UiDrawable::MakeRenderable()
	{
		assert(!attribs_.empty());
		renderer_->AddVertexFormat(vertex_format_, &attribs_[0], (U32)attribs_.size());
		renderer_->context()->CheckForErrors();
		
		renderer_->context()->GenVertexArrayObject(vertex_array_object_);
		renderer_->context()->BindVertexArrayObject(vertex_array_object_);
		renderer_->context()->CheckForErrors();
		
		renderer_->AddVertexBuffer(vertex_buffer_, num_vertices_ * vertex_format_->vertex_size(), vertices_array_, BufferUsage::kStaticDraw);
		renderer_->context()->CheckForErrors();
		if (vertex_buffer_ == nullptr)
		{
			can_render_ = false;
			return;
		}
		
		// Free all data in memory
		FreeArrays();
		
		// There is only one attribute
		const char* base = (char*)0;
		for (U32 i = 0; i < attribs_.size(); ++i)
		{
			const VertexFormat::Attrib& vf_attrib = vertex_format_->generic(i);
			renderer_->context()->VertexAttribPointer(i, vf_attrib.size, DataType::kFloat, vertex_format_->vertex_size(), base + vf_attrib.offset);
			renderer_->context()->CheckForErrors();
			renderer_->context()->EnableVertexAttribArray(i);
			renderer_->context()->CheckForErrors();
		}
		
		renderer_->context()->BindVertexArrayObject(0);
		renderer_->context()->CheckForErrors();
		
		attribs_.clear();
		attribs_.shrink_to_fit();
		
		can_render_ = true;
	}
	void UiDrawable::Render()
	{
		if (!can_render_)
			return;
		
		renderer_->ChangeTexture(texture_);
		renderer_->context()->BindVertexArrayObject(vertex_array_object_);
		renderer_->context()->DrawArrays(PrimitiveType::kTriangleStrip, 0, num_vertices_);
		renderer_->context()->BindVertexArrayObject(0);
	}
	Renderer * UiDrawable::renderer()
	{
		return renderer_;
	}

} // namespace scythe