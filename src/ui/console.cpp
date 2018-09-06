#include "console.h"

namespace scythe {

	Console::Console(Renderer * renderer, Font * font,
					 Shader * gui_shader, Shader * text_shader,
					 F32 bottom, F32 text_height, F32 velocity, F32 aspect_ratio)
	: vertical_board_(renderer, gui_shader, vec4(0.2f, 0.2f, 0.2f, 0.8f),
		aspect_ratio, // width
		1.0f-bottom, // height
		0.0f, // left
		bottom, // hmin
		1.0f, // hmax
		velocity,
		false, // is pos down
		true, // is vertical
		(U32)Flags::kRenderAlways)
	, labels_(nullptr)
	, need_to_update_input_(false)
	, need_to_update_all_(false)
	{
		max_lines_ = static_cast<U32>(vertical_board_.height()/text_height + 0.5f);
		assert(max_lines_ > 0);
		labels_ = new Label*[max_lines_];
		for (U32 i = 0; i < max_lines_; ++i)
		{
			lines_.push_back(L"");
			labels_[i] = new Label(renderer, text_shader, font, vec4(1.0f), text_height, 256,
											   0.0f, text_height*i, (U32)Flags::kRenderAlways);
			// Attach all labels to board widget
			vertical_board_.AttachWidget(labels_[i]);
		}
	}
	Console::~Console()
	{
		delete[] labels_;
	}
	bool Console::IsActive()
	{
		return vertical_board_.IsPosMin();
	}
	void Console::Move()
	{
		vertical_board_.Move();
	}
	void Console::AddString(const std::wstring& text)
	{
		for (U32 i = 0; i < max_lines_-2; ++i)
		{
			U32 index = max_lines_ - 1 - i;
			U32 index_next = index - 1;
			lines_[index] = lines_[index_next];
		}
		lines_[1] = text;
		need_to_update_all_ = true;
	}
	void Console::Update(F32 sec)
	{
		if (need_to_update_all_)
		{
			for (U32 i = 0; i < max_lines_; ++i)
			{
				labels_[i]->SetText(lines_[i].c_str());
			}
			need_to_update_all_ = false;
		}
		else if (need_to_update_input_)
		{
			labels_[0]->SetText(input_string().c_str());
			need_to_update_input_ = false;
		}
		vertical_board_.UpdateAll(sec);
	}
	void Console::Render()
	{
		if (vertical_board_.IsPosMin())
			vertical_board_.RenderAll(); // render entire tree
		else
			vertical_board_.Render(); // render only console rect (smart hack for labels :D)
	}
	void Console::ProcessKeyInput(PublicKey key, int mods)
	{
		if ((key == PublicKey::kGraveAccent) && !(mods & ModifierKey::kShift))
		{
			Move();
		}
		else if (key == PublicKey::kBackspace)
		{
			RemoveSymbol();
		}
		else if (key == PublicKey::kEnter)
		{
			PushString();
		}
	}
	void Console::ProcessCharInput(unsigned short code)
	{
		wchar_t symbol = static_cast<wchar_t>(code);
		InsertSymbol(symbol);
	}
	std::wstring& Console::input_string()
	{
		return lines_[0];
	}
	void Console::InsertSymbol(wchar_t symbol)
	{
		input_string().push_back(symbol);
		need_to_update_input_ = true;
	}
	void Console::RemoveSymbol()
	{
		if (!input_string().empty())
		{
			input_string().pop_back();
			need_to_update_input_ = true;
		}
	}
	void Console::PushString()
	{
		RecognizeString();
		InsertString();
	}
	void Console::InsertString()
	{
		for (U32 i = 0; i < max_lines_-1; ++i)
		{
			U32 index = max_lines_ - 1 - i;
			U32 index_next = index - 1;
			lines_[index] = lines_[index_next];
		}
		lines_[0].clear();
		need_to_update_all_ = true;
	}
	void Console::RecognizeString()
	{
		// Inheritants should override this function
	}

} // namespace scythe