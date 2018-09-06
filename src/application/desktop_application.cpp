#include "desktop_application.h"

#include "platform/desktop_main_wrapper.h"
#include "platform/window_controller.h"
#include "graphics/renderer.h"
#include "stream/file_stream.h"

#include <cstdlib>
#include <clocale>

namespace scythe {

	DesktopApplication::DesktopApplication()
	: Application()
	, visible_(false)
	, fullscreen_(false)
	{

	}
	DesktopApplication::~DesktopApplication()
	{

	}
	int DesktopApplication::Run(int argc, const char** argv)
	{
		app_ = this;
		
		// Set proper text encoding to let use non-english characters
		setlocale(LC_CTYPE, "UTF-8");

		// Prestart initialization
		if (!PreStartInit())
		{
			return 1;
		}

		// Read window settings from file
		FileStream ini_file;
		if (ini_file.Open(GetIniFilePath(), StreamAccess::kReadText))
		{
			char temp[10]; int w, h; bool fullscr;

			ini_file.ScanString("%i x %i", &w, &h);
			ini_file.ScanString("%s", temp, _countof(temp));
			ini_file.Close();

			fullscr = strcmp(temp, "TRUE") == 0;

			InitWindowSize(w, h, fullscr);
		}
		else
		{
			InitWindowSize(800, 600, false);
		}
		
		// This code is for Mac OS X and iOS.
		// Under these platforms all initializing code doing in coressponding classes.
		PlatformChangeDirectoryToResources();

		// Wrapper for the Mac OS X and iOS main function
		return DesktopMainWrapper(argc, argv);
	}
	void DesktopApplication::Terminate()
	{
		PlatformWindowTerminate();
	}
	const char* DesktopApplication::GetIniFilePath()
	{
		return "config.ini";
	}
	void DesktopApplication::ToggleFullscreen(void)
	{
		if (fullscreen_) // fullscreen -> windowed
		{
			MakeWindowed();
			fullscreen_ = false;
		}
		else // windowed -> fullscreen
		{
			if (MakeFullscreen())
				fullscreen_ = true;
			else
				fullscreen_ = false;
		}
	}
	bool DesktopApplication::MakeFullscreen(void)
	{
		if (fullscreen_) return true;
		
		return PlatformWindowMakeFullscreen();
	}
	void DesktopApplication::MakeWindowed(void)
	{
		if (!fullscreen_) return;

		PlatformWindowMakeWindowed();
	}
	void DesktopApplication::Center()
	{
		PlatformWindowCenter();
	}
	void DesktopApplication::Resize(int width, int height)
	{
		PlatformWindowResize(width, height);
	}
	void DesktopApplication::SetTitle(const char* title)
	{
		PlatformWindowSetTitle(title);
	}
	void DesktopApplication::Iconify()
	{
		PlatformWindowIconify();
	}
	void DesktopApplication::Restore()
	{
		PlatformWindowRestore();
	}
	void DesktopApplication::Show()
	{
		PlatformWindowShow();
	}
	void DesktopApplication::Hide()
	{
		PlatformWindowHide();
	}
	void DesktopApplication::InitWindowSize(int w, int h, bool fullscr)
	{
		width_ = w;
		height_ = h;
		aspect_ratio_ = (float)width_ / (float)height_;
		fullscreen_ = fullscr;
		ComputeFramebufferSize();
	}
	void DesktopApplication::GetCursorPos(float& x, float& y)
	{
		PlatformGetCursorPos(x, y);
	}
	void DesktopApplication::SetCursorPos(float x, float y)
	{
		PlatformSetCursorPos(x, y);
	}
	void DesktopApplication::CursorToCenter()
	{
		PlatformMouseToCenter();
	}
	void DesktopApplication::ShowCursor()
	{
		PlatformShowCursor();
	}
	void DesktopApplication::HideCursor()
	{
		PlatformHideCursor();
	}
	const bool DesktopApplication::IsMultisample()
	{
		return false;
	}
	const bool DesktopApplication::IsBenchmark()
	{
		return false;
	}
	const bool DesktopApplication::IsResizable()
	{
		return false;
	}
	const bool DesktopApplication::IsDecorated()
	{
		return true;
	}
	void DesktopApplication::OnChar(unsigned short code)
	{
	}
	void DesktopApplication::OnKeyDown(PublicKey key, int modifiers)
	{
	}
	void DesktopApplication::OnKeyUp(PublicKey key, int modifiers)
	{
	}
	void DesktopApplication::OnMouseDown(MouseButton button, int modifiers)
	{
	}
	void DesktopApplication::OnMouseUp(MouseButton button, int modifiers)
	{
	}
	void DesktopApplication::OnMouseMove()
	{
	}
	void DesktopApplication::OnScroll(float delta_x, float delta_y)
	{
	}
	void DesktopApplication::OnSize(int w, int h)
	{
		width_ = w;
		height_ = h;
		aspect_ratio_ = (float)width_ / (float)height_;
		if (renderer_) // renderer may not be initialized yet
		{
			renderer_->UpdateSizes(width_, height_);
			renderer_->SetViewport(width_, height_);
		}
		// TODO: what to do if framebuffer size changes during executing?
		ComputeFramebufferSize();
	}
	bool DesktopApplication::visible()
	{
		return visible_;
	}
	bool DesktopApplication::fullscreen()
	{
		return fullscreen_;
	}
	void DesktopApplication::set_visible(bool vis)
	{
		visible_ = vis;
	}
	Keys& DesktopApplication::keys()
	{
		return keys_;
	}
	Mouse& DesktopApplication::mouse()
	{
		return mouse_;
	}

} // namespace scythe