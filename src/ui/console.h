#ifndef __SCYTHE_CONSOLE_H__
#define __SCYTHE_CONSOLE_H__

#include "board.h"
#include "label.h"
#include "input/keys.h"

#include <string>
#include <vector>

namespace scythe {

	//! Standart console class
	// Has initial state at top of the window (lower bound at top)
	// Supports only english charaters in input
	class Console {
	public:
		//! Requires gui colored shader to render
		Console(sht::graphics::Renderer * renderer, sht::graphics::Font * font,
				sht::graphics::Shader * gui_shader, sht::graphics::Shader * text_shader,
				F32 bottom, F32 text_height, F32 velocity, F32 aspect_ratio);
		virtual ~Console();

		bool IsActive(); //!< down position
		void Move();
		void AddString(const std::wstring& text);
		
		void Update(F32 sec);
		void Render();
		
		void ProcessKeyInput(sht::PublicKey key, int mods);
		void ProcessCharInput(unsigned short code);

	protected:
		std::wstring& input_string();
		virtual void RecognizeString();

	private:
		Console(const Console&) = delete;
		Console& operator =(const Console&) = delete;
		
		void InsertSymbol(wchar_t symbol);
		void RemoveSymbol();
		void PushString();
		void InsertString();
		
		ui::ColoredBoard vertical_board_;
		U32 max_lines_; //!< maximum number of lines
		ui::Label * * labels_; // pointers to labels
		std::vector<std::wstring> lines_;
		bool need_to_update_input_;
		bool need_to_update_all_;
	};

} // namespace scythe

#endif