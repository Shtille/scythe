#include "label.h"

namespace scythe {

	Label::Label(Renderer * renderer, Shader * shader,
				 Font * font, const vec4& color, F32 text_height, U32 buffer_size,
				 F32 x, F32 y, U32 flags)
	: Widget(x, y, flags)
	, shader_(shader)
	, font_(font)
	, color_(color)
	, text_height_(text_height)
	, constant_position_(false)
	{
		BindConstUniforms();
		text_ = DynamicText::Create(renderer, buffer_size);
		assert(text_);
		SetText(L""); // just to be able to render the text
	}
	Label::~Label()
	{
		delete text_;
	}
	void Label::set_constant_position(bool use)
	{
		constant_position_ = use;
	}
	void Label::Render()
	{
		if (!constant_position_)
		{
			// Doing this at Render() because we sometimes omit rendering labels
			vec2 position;
			ObtainGlobalPosition(&position);
			text_->SetPosition(position);
		}

		shader_->Bind();
		shader_->Uniform4fv("u_color", color_);
		text_->Render();
		shader_->Unbind();
	}
	void Label::AppendSymbol(wchar_t symbol)
	{
		text_->AppendSymbol(symbol);
	}
	void Label::SetText(const wchar_t* text)
	{
		vec2 position;
		ObtainGlobalPosition(&position);
		text_->SetTextSimple(font_, position.x, position.y, text_height_, text);
	}
	void Label::AlignCenter(F32 rect_width, F32 rect_height)
	{
		if (parent_ == nullptr) return;

		vec2 parent_position;
		parent_->ObtainGlobalPosition(&parent_position);

		// We also have to modify local position
		float min_x, min_y, max_x, max_y;
		text_->GetTextBoundingBox(&min_x, &min_y, &max_x, &max_y);
		position_.x = 0.5f * rect_width - 0.5f * (max_x - min_x);
		position_.y = 0.5f * rect_height - 0.5f * (max_y - min_y);

		vec2 position;
		position.x = parent_position.x + position_.x;
		position.y = parent_position.y + position_.y;
		text_->SetPosition(position);
	}
	void Label::BindConstUniforms()
	{
		shader_->Bind();
		shader_->Uniform1i("u_texture", 0);
		shader_->Unbind();
	}

} // namespace scythe