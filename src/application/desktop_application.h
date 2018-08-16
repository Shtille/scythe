#ifndef __SCYTHE_DESKTOP_APPLICATION_H__
#define __SCYTHE_DESKTOP_APPLICATION_H__

#include "application.h"

#include "input/keys.h"
#include "input/mouse.h"

namespace scythe {

	//! Class for desktop application
	class DesktopApplication : public Application {
	public:
		DesktopApplication();
		virtual ~DesktopApplication();
		
		const char* GetIniFilePath();

		int Run(int argc, const char** argv);
		void Terminate();

		// Window operations
		void ToggleFullscreen(void);
		bool MakeFullscreen(void);
		void MakeWindowed(void);
		void Center();
		void Resize(int width, int height);
		void SetTitle(const char* title);
		void Iconify();
		void Restore();
		void Show();
		void Hide();
		
		// Cursor operations
		void GetCursorPos(float& x, float& y); // x=[0,w]; y=[0,h]
		void SetCursorPos(float x, float y); // x=[0,w]; y=[0,h]
		void CursorToCenter();
		void ShowCursor();
		void HideCursor();

		bool visible();
		bool fullscreen();
		
		Keys& keys();
		Mouse& mouse();

		void set_visible(bool vis);

		// Application parameters
		virtual const bool IsMultisample();
		virtual const bool IsBenchmark(); //!< allows to get maximum available FPS (Windows only)
		virtual const bool IsResizable(); //!< window style is resizable
		virtual const bool IsDecorated(); //!< window style is decorated

		// --- Messages ---
		virtual void OnChar(unsigned short code);
		virtual void OnKeyDown(PublicKey key, int modifiers);
		virtual void OnKeyUp(PublicKey key, int modifiers);
		virtual void OnMouseDown(MouseButton button, int modifiers);
		virtual void OnMouseUp(MouseButton button, int modifiers);
		virtual void OnMouseMove();
		virtual void OnScroll(float delta_x, float delta_y);
		virtual void OnSize(int w, int h);

	protected:

		Keys keys_;                     //!< keys information
		Mouse mouse_;                   //!< mouse information
		bool visible_;					//!< is window visible
		bool fullscreen_;				//!< is window fullscreen

	private:
		
		void InitWindowSize(int w, int h, bool fullscr);
	};

} // namespace scythe

#endif