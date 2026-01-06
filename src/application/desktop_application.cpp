#include <scythe/desktop_application.h>

#include <scythe/graphics_provider.h>

#include "../platform/platform_inner.h"

static constexpr int kApplicationWidth = 800;
static constexpr int kApplicationHeight = 600;
static constexpr bool kApplicationFullscreen = false;

namespace scythe {

	DesktopApplication::DesktopApplication()
	: Application()
	, keyboard_state_()
	, mouse_state_()
	, keyboard_controller_(nullptr)
	, mouse_controller_(nullptr)
	, platform_window_(nullptr)
	{
	}
	DesktopApplication* DesktopApplication::GetInstance()
	{
		return Application::GetInstance()->Upcast<DesktopApplication>();
	}
	KeyboardState& DesktopApplication::GetKeyboardState()
	{
		return keyboard_state_;
	}
	MouseState& DesktopApplication::GetMouseState()
	{
		return mouse_state_;
	}
	KeyboardController* DesktopApplication::GetKeyboardController()
	{
		return keyboard_controller_;
	}
	MouseController* DesktopApplication::GetMouseController()
	{
		return mouse_controller_;
	}
	bool DesktopApplication::CreateSurface()
	{
		if (platform::window::Create())
		{
			platform::window::Center();
			return true;
		}
		else
			return false;
	}
	void DesktopApplication::DestroySurface()
	{
		platform::window::Destroy();
	}
	const Window* DesktopApplication::GetWindow() const
	{
		return platform::GetWindowByPlatformOne(platform_window_);
	}
	const int DesktopApplication::GetWidth() const
	{
		const Window* window = GetWindow();
		return window->width;
	}
	const int DesktopApplication::GetHeight() const
	{
		const Window* window = GetWindow();
		return window->height;
	}
	const float DesktopApplication::GetAspectRatio() const
	{
		const Window* window = GetWindow();
		return window->aspect_ratio;
	}
	const bool DesktopApplication::IsVisible() const
	{
		const Window* window = GetWindow();
		return window->visible;
	}
	const bool DesktopApplication::IsFullscreen() const
	{
		const Window* window = GetWindow();
		return window->fullscreen;
	}
	const int DesktopApplication::GetInitialWidth() const
	{
		return kApplicationWidth;
	}
	const int DesktopApplication::GetInitialHeight() const
	{
		return kApplicationHeight;
	}
	const bool DesktopApplication::GetInitialFullscreen() const
	{
		return kApplicationFullscreen;
	}
	const bool DesktopApplication::IsDecorated() const
	{
		return true;
	}
	const bool DesktopApplication::IsResizable() const
	{
		return false;
	}
	void DesktopApplication::OnSize(int width, int height)
	{
	}

}