#include "../platform_inner.h"

#include <cstring>
#include <Windowsx.h> // for GET_X_LPARAM

#include <scythe/desktop_application.h>
#include <scythe/log.h>

#include "platform_data.h"

#ifndef WM_MOUSEHWHEEL // It isn't defined on MinGW platform
# define WM_MOUSEHWHEEL 0x020E
#endif

// Avoid conflictable Windows defines:
#ifdef CreateDirectory
# undef CreateDirectory
#endif // CreateDirectory

#ifdef RemoveDirectory
# undef RemoveDirectory
#endif // RemoveDirectory

static constexpr char* kMainWindowClassName = "scythe-window";
static constexpr char* kHelperWindowClassName = "scythe-helper-window";

// Translates Windows key modifiers to engine ones
static scythe::KeyModifiers TranslateKeyboardModifiers(void)
{
	scythe::KeyModifiers modifiers;
	if (::GetKeyState(VK_SHIFT) & (1 << 31))
		modifiers |= scythe::KeyboardKeyModifier::kShift;
	if (::GetKeyState(VK_CONTROL) & (1 << 31))
		modifiers |= scythe::KeyboardKeyModifier::kControl;
	if (::GetKeyState(VK_MENU) & (1 << 31))
		modifiers |= scythe::KeyboardKeyModifier::kAlt;
	if ((::GetKeyState(VK_LWIN) | ::GetKeyState(VK_RWIN)) & (1 << 31))
		modifiers |= scythe::KeyboardKeyModifier::kSuper;
	return modifiers;
}

static scythe::KeyboardKey TranslateKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_CONTROL)
	{
		// The CTRL keys require special handling

		MSG next;
		DWORD time;

		// Is this an extended key (i.e. right key)?
		if (lParam & 0x01000000)
			return scythe::KeyboardKey::kRightControl;

		// Here is a trick: "Alt Gr" sends LCTRL, then RALT. We only
		// want the RALT message, so we try to see if the next message
		// is a RALT message. In that case, this is a false LCTRL!
		time = ::GetMessageTime();

		if (::PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
		{
			if (next.message == WM_KEYDOWN ||
				next.message == WM_SYSKEYDOWN ||
				next.message == WM_KEYUP ||
				next.message == WM_SYSKEYUP)
			{
				if (next.wParam == VK_MENU &&
					(next.lParam & 0x01000000) &&
					next.time == time)
				{
					// Next message is a RALT down message, which
					// means that this is not a proper LCTRL message
					return scythe::KeyboardKey::kUnknown;
				}
			}
		}

		return scythe::KeyboardKey::kLeftControl;
	}

	scythe::DesktopApplication* desktop_app = scythe::DesktopApplication::GetInstance();
	scythe::KeyboardState& keyboard_state = desktop_app->GetKeyboardState();
	scythe::KeyCode code = static_cast<scythe::KeyCode>(HIWORD(lParam) & 0x1FF);
	return keyboard_state.GetKeyByCode(code);
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	scythe::DesktopApplication* desktop_app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(desktop_app);
	scythe::platform::Window* window = data->main_window;
	scythe::KeyboardState& keyboard_state = desktop_app->GetKeyboardState();
	scythe::MouseState& mouse_state = desktop_app->GetMouseState();
	scythe::KeyboardController* keyboard_controller = desktop_app->GetKeyboardController();
	scythe::MouseController* mouse_controller = desktop_app->GetMouseController();
	scythe::WindowController* window_controller = desktop_app->GetWindowController();

	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_SCREENSAVE:		// Screensaver Trying To Start?
		case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
			return 0;				// Prevent From Happening

		case SC_KEYMENU:        // Changing focus to system menu
			return 0;				// Prevent From Happening
		}
		break;

	case WM_CREATE:	// Window Creation
					// Initialize mouse position
		::GetCursorPos(&window->old_mouse_position);
		::ScreenToClient(hWnd, &window->old_mouse_position);
		return 0;

	case WM_CLOSE:	// Closing The Window
		::DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	case WM_SETFOCUS: // Got focus
		window->base.active = true;
		if (window_controller)
			window_controller->OnGetFocus();
		return 0;
	case WM_KILLFOCUS: // Focus is lost
		window->base.active = false;
		if (window_controller)
			window_controller->OnLostFocus();
		return 0;

	case WM_SIZE:	// Size Action Has Taken Place
		switch (wParam)
		{
		case SIZE_MINIMIZED:
			window->base.visible = false;
			if (window_controller)
				window_controller->OnMinimized();
			return 0;

		case SIZE_MAXIMIZED:
			window->base.visible = true;
			if (window_controller)
				window_controller->OnMaximized();
			return 0;

		case SIZE_RESTORED:
		{
			int width = static_cast<int>(LOWORD(lParam));
			int height = static_cast<int>(HIWORD(lParam));
			window->base.visible = true;
			window->base.width = width;
			window->base.height = height;
			window->base.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
			if (window_controller)
				window_controller->OnResize(width, height);
			return 0;
		}
		}
		break;

	case WM_CHAR:
	{
		scythe::KeyCode code = static_cast<scythe::KeyCode>(wParam);
		if (keyboard_controller && scythe::IsGoodChar(code))
			keyboard_controller->OnChar(code);
		return 0;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		const scythe::KeyboardKey translated_key = ::TranslateKey(wParam, lParam);
		if (translated_key == scythe::KeyboardKey::kUnknown)
			break;
		const scythe::KeyModifiers modifiers = ::TranslateKeyboardModifiers();

		keyboard_state.SetKeyDown(translated_key, true);
		keyboard_state.modifiers = modifiers;

		if (keyboard_controller)
			keyboard_controller->OnKeyDown(translated_key, modifiers);
		break;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		const scythe::KeyboardKey translated_key = ::TranslateKey(wParam, lParam);
		if (translated_key == scythe::KeyboardKey::kUnknown)
			break;
		const scythe::KeyModifiers modifiers = ::TranslateKeyboardModifiers();

		keyboard_state.SetKeyDown(translated_key, false);
		keyboard_state.SetKeyActive(translated_key, false);
		keyboard_state.modifiers = modifiers;

		if (wParam == VK_SHIFT)
		{
			// Release both Shift keys on Shift up event, as only one event
			// is sent even if both keys are released
			if (keyboard_controller)
			{
				keyboard_controller->OnKeyUp(scythe::KeyboardKey::kLeftShift, modifiers);
				keyboard_controller->OnKeyUp(scythe::KeyboardKey::kRightShift, modifiers);
			}
		}
		else if (wParam == VK_SNAPSHOT)
		{
			// Key down is not reported for the print screen key
			if (keyboard_controller)
			{
				keyboard_controller->OnKeyDown(scythe::KeyboardKey::kPrintScreen, modifiers);
				keyboard_controller->OnKeyUp(scythe::KeyboardKey::kPrintScreen, modifiers);
			}
		}
		else
		{
			if (keyboard_controller)
				keyboard_controller->OnKeyUp(translated_key, modifiers);
		}

		break;
	}

	case WM_LBUTTONDOWN:
		keyboard_state.modifiers = ::TranslateKeyboardModifiers();
		mouse_state.SetButtonDown(scythe::MouseButton::kLeft, true);
		if (mouse_controller)
			mouse_controller->OnMouseDown(scythe::MouseButton::kLeft);
		return 0;
	case WM_LBUTTONUP:
		keyboard_state.modifiers = ::TranslateKeyboardModifiers();
		mouse_state.SetButtonDown(scythe::MouseButton::kLeft, false);
		if (mouse_controller)
			mouse_controller->OnMouseUp(scythe::MouseButton::kLeft);
		return 0;

	case WM_MBUTTONDOWN:
		keyboard_state.modifiers = ::TranslateKeyboardModifiers();
		mouse_state.SetButtonDown(scythe::MouseButton::kMiddle, true);
		if (mouse_controller)
			mouse_controller->OnMouseDown(scythe::MouseButton::kMiddle);
		return 0;
	case WM_MBUTTONUP:
		keyboard_state.modifiers = ::TranslateKeyboardModifiers();
		mouse_state.SetButtonDown(scythe::MouseButton::kMiddle, false);
		if (mouse_controller)
			mouse_controller->OnMouseUp(scythe::MouseButton::kMiddle);
		return 0;

	case WM_RBUTTONDOWN:
		keyboard_state.modifiers = ::TranslateKeyboardModifiers();
		mouse_state.SetButtonDown(scythe::MouseButton::kRight, true);
		if (mouse_controller)
			mouse_controller->OnMouseDown(scythe::MouseButton::kRight);
		return 0;
	case WM_RBUTTONUP:
		keyboard_state.modifiers = ::TranslateKeyboardModifiers();
		mouse_state.SetButtonDown(scythe::MouseButton::kRight, false);
		if (mouse_controller)
			mouse_controller->OnMouseUp(scythe::MouseButton::kRight);
		return 0;

	case WM_MOUSEMOVE:
	{
		POINT mouse_position;
		mouse_position.x = GET_X_LPARAM(lParam);
		mouse_position.y = GET_Y_LPARAM(lParam);
		if (mouse_position.x != window->old_mouse_position.x ||
			mouse_position.y != window->old_mouse_position.y)
		{
			mouse_state.delta_x = static_cast<float>(mouse_position.x - window->old_mouse_position.x);
			mouse_state.delta_y = static_cast<float>(mouse_position.y - window->old_mouse_position.y);
			window->old_mouse_position = mouse_position;

			RECT client_rect;
			::GetClientRect(hWnd, &client_rect);
			mouse_state.x = static_cast<float>(mouse_position.x);
			mouse_state.y = static_cast<float>(client_rect.bottom - mouse_position.y - 1);
			if (mouse_controller)
				mouse_controller->OnMouseMove();
		}
		return 0;
	}

	case WM_MOUSEWHEEL:
		if (mouse_controller)
			mouse_controller->OnScroll(0.0f, (float)HIWORD(wParam) / (float)WHEEL_DELTA);
		return 0;
	case WM_MOUSEHWHEEL:
		if (mouse_controller)
			mouse_controller->OnScroll(-((float)HIWORD(wParam) / (float)WHEEL_DELTA), 0.0f);
		return 0;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);	// Pass Unhandled Messages
}
static void AdjustWindowedAppearance(scythe::platform::Window* window, scythe::DesktopApplication* app, bool windowed)
{
	if (windowed)
	{
		if (app->IsDecorated())
		{
			window->current_state.style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

			if (app->IsResizable())
			{
				window->current_state.style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
				window->current_state.ex_style |= WS_EX_WINDOWEDGE;
			}
		}
		else
		{
			window->current_state.style |= WS_POPUP;
		}
		// Adjust Window, Account For Window Borders
		::AdjustWindowRectEx(&window->current_state.rect, window->current_state.style, 0, window->current_state.ex_style);
	}
}
static void ConfigureWindowSettings(scythe::platform::Window* window, scythe::DesktopApplication* app)
{
	// Base properties
	window->base.width = app->GetInitialWidth();
	window->base.height = app->GetInitialHeight();
	window->base.aspect_ratio = static_cast<float>(window->base.width) / static_cast<float>(window->base.height);
	window->base.active = true;
	window->base.visible = false;
	window->base.fullscreen = app->GetInitialFullscreen();
	window->old_mouse_position = { 0 };

	// Style
	window->current_state.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	window->current_state.ex_style = WS_EX_APPWINDOW;

	window->current_state.rect.left = 0;
	window->current_state.rect.top = 0;
	window->current_state.rect.right = window->base.width;
	window->current_state.rect.bottom = window->base.height;

	::AdjustWindowedAppearance(window, app, !window->base.fullscreen);
}
static void EnterFullscreenMode(scythe::platform::Window* window, scythe::DesktopApplication* app)
{
	bool windowed = true;
	if (window->base.fullscreen)	// Fullscreen Requested, Try Changing Video Modes
	{
		if (!scythe::platform::window::MakeFullscreen(true))
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			window->base.fullscreen = false;
			scythe::Error("Mode Switch Failed. Running In Windowed Mode.");
		}
		else // Otherwise (If Fullscreen Mode Was Successful)
		{
			windowed = false;
			window->current_state.style |= WS_POPUP;
			window->current_state.ex_style |= WS_EX_TOPMOST;
		}
	}
	::AdjustWindowedAppearance(window, app, windowed);
}
static void LeaveFullscreenMode()
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	if (data->main_window->base.fullscreen)
		::ChangeDisplaySettingsA(NULL, 0);
}
static LRESULT CALLBACK HelperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
}
static bool CreateHelperWindow(scythe::platform::Data* data)
{
	MSG msg;
	WNDCLASSEXA wc = { sizeof(wc) };

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = (WNDPROC) HelperWindowProc;
	wc.hInstance     = data->instance;
	wc.lpszClassName = kHelperWindowClassName;

	data->helper_window_class_registered = ::RegisterClassExA(&wc) != 0;
	if (!data->helper_window_class_registered)
	{
		scythe::Error("Failed to register helper window class");
		return false;
	}

	data->helper_window_handle = ::CreateWindowExA(
		WS_EX_OVERLAPPEDWINDOW,
		kHelperWindowClassName,
		"scythe helper window",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 1, 1,
		NULL, NULL,
		data->instance,
		NULL);

	if (!data->helper_window_handle)
	{
		scythe::Error("Failed to create helper window");
		return false;
	}

	// HACK: The command to the first ShowWindow call is ignored if the parent
	//       process passed along a STARTUPINFO, so clear that with a no-op call
	::ShowWindow(data->helper_window_handle, SW_HIDE);

	while (::PeekMessageA(&msg, data->helper_window_handle, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessageA(&msg);
	}

	return true;
}
static void DestroyHelperWindow(scythe::platform::Data* data)
{
	if (data->helper_window_handle)
		::DestroyWindow(data->helper_window_handle);
	if (data->helper_window_class_registered)
		::UnregisterClassA(kHelperWindowClassName, data->instance);
}
static inline scythe::platform::Window* GetMainWindow()
{
	scythe::Application* app = scythe::Application::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	return data->main_window;
}

namespace scythe {

	const BaseWindow* GetBaseWindow(const Application* app)
	{
		const platform::Window* window = platform::GetWindow(app);
		return &window->base;
	}

	namespace platform {

		Data* CreateData()
		{
			Data* data = new Data();
			data->instance = nullptr;
			data->icon = nullptr;
			data->helper_window_class_registered = false;
			data->main_window_class_registered = false;
			data->helper_window_handle = nullptr;
			data->main_window = nullptr;
			return data;
		}
		void DestroyData(Data* data)
		{
			delete data;
		}
		Data* GetData(Application* app)
		{
			return reinterpret_cast<Data*>(app->platform_data_);
		}
		const Data* GetData(const Application* app)
		{
			return reinterpret_cast<const Data*>(app->platform_data_);
		}
		const Window* GetWindow(const Application* app)
		{
			const Data* data = GetData(app);
			return data->main_window;
		}

		bool Initialize()
		{
			Application* app = Application::GetInstance();
			Data* data = GetData(app);

			// Skip any upcoming error
			::GetLastError();

			//	Some dual core systems have a problem where the different CPUs return different
			//	QueryPerformanceCounter values. So when this thread is schedule on the other CPU
			//	in a later frame, we could even get a negative frameTime. To solve this we force
			//	the main thread to always run on CPU 0.
			::SetThreadAffinityMask(::GetCurrentThread(), 1);

			// This is the way to change window icon manually:
			data->icon = ::LoadIcon(NULL, IDI_APPLICATION);
			if (data->icon == NULL)
			{
				Error("Icon loading failed!");
				return false;
			}

			data->instance = GetModuleHandle(NULL);

			// Register A Window Class
			WNDCLASSEXA wc;
			::ZeroMemory(&wc, sizeof(WNDCLASSEXA));
			wc.cbSize = sizeof(WNDCLASSEXA);
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc = (WNDPROC)(WindowProc);
			wc.hInstance = data->instance;
			wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
			wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
			wc.hIcon = data->icon;
			wc.hIconSm = data->icon;
			wc.lpszClassName = kMainWindowClassName;

			data->main_window_class_registered = ::RegisterClassExA(&wc) != 0;
			if (!data->main_window_class_registered)
			{
				Error("RegisterClassEx Failed!");
				return false;
			}

			if (!::CreateHelperWindow(data))
			{
				Error("CreateHelperWindow Failed!");
				return false;
			}
			return true;
		}
		void Deinitialize()
		{
			Application* app = Application::GetInstance();
			Data* data = GetData(app);

			::DestroyHelperWindow(data);

			if (data->main_window_class_registered)
				::UnregisterClassA(kMainWindowClassName, data->instance);
			if (data->icon)
				::DestroyIcon(data->icon);
		}

		void PollEvents()
		{
			Application* app = Application::GetInstance();
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				// Handle or dispatch messages
				if (msg.message == WM_QUIT)
				{
					//app->need_quit_ = true;
					app->Terminate();
				}
				else
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
		}
		void ChangeDirectoryToResources()
		{
			char buffer[MAX_PATH];
			::GetCurrentDirectoryA(MAX_PATH, buffer);
			::lstrcatA(buffer, TEXT("\\.."));
			::SetCurrentDirectoryA(buffer);
		}

		// Window functions
		namespace window {

			bool Create()
			{
				DesktopApplication* app = DesktopApplication::GetInstance();
				Data* data = GetData(app);

				// Create window instance
				Window* window = new Window();
				data->main_window = window;
				if (!data->main_window)
					return false;

				// Configure window size
				::ConfigureWindowSettings(window, app);

				// Create Window
				window->handle = ::CreateWindowExA(
					window->current_state.ex_style,											// Extended Style
					kMainWindowClassName,													// Class Name
					app->GetTitle(),														// Window Title
					window->current_state.style,											// Window Style
					0, 0,																	// Window X,Y Position
					window->current_state.rect.right - window->current_state.rect.left,		// Window Width
					window->current_state.rect.bottom - window->current_state.rect.top,		// Window Height
					HWND_DESKTOP,															// Desktop Is Window's Parent
					0,																		// No Menu
					data->instance,															// Pass The Window Instance
					nullptr);																// pointer to window class
				if (!window->handle)
				{
					Error("CreateWindowExA Failed!");
					return false;
				}

				// Enter fullscreen if necessary and adjust borders
				::EnterFullscreenMode(window, app);

				return true;
			}
			void Destroy()
			{
				Application* app = Application::GetInstance();
				Data* data = GetData(app);

				// Leave fullscreen mode (if it hasn't been windowed)
				::LeaveFullscreenMode();

				// Destroy main window
				if (data->main_window->handle)
				{
					::DestroyWindow(data->main_window->handle);
					data->main_window->handle = nullptr;
				}

				// Delete its instance
				if (data->main_window)
				{
					delete data->main_window;
					data->main_window = nullptr;
				}
			}
			void Terminate()
			{
				Window* window = ::GetMainWindow();
				::PostMessage(window->handle, WM_CLOSE, 0, 0);
			}

			void ToggleFullscreen(void)
			{
				Window* window = ::GetMainWindow();
				if (window->base.fullscreen) // fullscreen -> windowed
				{
					MakeWindowed();
					window->base.fullscreen = false;
				}
				else // windowed -> fullscreen
				{
					window->base.fullscreen = MakeFullscreen();
				}
			}
			bool MakeFullscreen(bool force)
			{
				Application* app = Application::GetInstance();
				Data* data = GetData(app);
				Window* window = data->main_window;

				if (window->base.fullscreen && !force)
					return true;

				int width = window->base.width;
				int height = window->base.height;
				int color_bits = app->GetColorBits();

				// Store current window state:
				window->old_state.rect = window->current_state.rect;
				window->old_state.style = window->current_state.style;
				window->old_state.ex_style = window->current_state.ex_style;
				
				DEVMODE dmScreenSettings;
				::ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
				dmScreenSettings.dmSize = sizeof(DEVMODE);
				dmScreenSettings.dmPelsWidth = width;
				dmScreenSettings.dmPelsHeight = height;
				dmScreenSettings.dmBitsPerPel = color_bits;
				dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				if (::ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					// Still windowed
					::SetWindowLongPtr(window->handle, GWL_STYLE, window->old_state.style);
					::SetWindowLongPtr(window->handle, GWL_EXSTYLE, window->old_state.ex_style);
					::ShowWindow(window->handle, SW_NORMAL);
					return false;
				}

				::SetWindowLongPtr(window->handle, GWL_STYLE, WS_POPUP);
				::SetWindowLongPtr(window->handle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
				::MoveWindow(window->handle, 0, 0, width, height, FALSE);
				::ShowWindow(window->handle, SW_NORMAL);
				return true;
			}
			void MakeWindowed(bool force)
			{
				Window* window = ::GetMainWindow();

				if (!window->base.fullscreen && !force)
					return;

				::ChangeDisplaySettings(NULL, 0); // restore display settings

				// Restore window state
				::SetWindowLongPtr(window->handle, GWL_STYLE, window->old_state.style);
				::SetWindowLongPtr(window->handle, GWL_EXSTYLE, window->old_state.ex_style);
				::MoveWindow(window->handle, window->old_state.rect.left, window->old_state.rect.top,
					window->old_state.rect.right - window->old_state.rect.left,
					window->old_state.rect.bottom - window->old_state.rect.top, FALSE);
				::ShowWindow(window->handle, SW_NORMAL);
			}
			void Center()
			{
				Window* window = ::GetMainWindow();
				RECT window_rect;
				::GetWindowRect(window->handle, &window_rect);
				int screen_width = ::GetSystemMetrics(SM_CXSCREEN);
				int screen_height = ::GetSystemMetrics(SM_CYSCREEN);
				int window_width = window_rect.right - window_rect.left;
				int window_height = window_rect.bottom - window_rect.top;
				window_rect.left = (screen_width - window_width)/2;
				window_rect.top = (screen_height - window_height)/2;
				::MoveWindow(window->handle, window_rect.left, window_rect.top, window_width, window_height, TRUE);
			}
			void Resize(int width, int height)
			{
				Window* window = ::GetMainWindow();
				RECT rect;
				POINT pos;
				pos.x = width;
				pos.y = height;
				::GetWindowRect(window->handle, &rect);
				::ClientToScreen(window->handle, &pos);
				::MoveWindow(window->handle, rect.left, rect.top, pos.x - rect.left, pos.y - rect.top, TRUE);
			}
			void Iconify()
			{
				Window* window = ::GetMainWindow();
				::ShowWindow(window->handle, SW_MINIMIZE);
			}
			void Restore()
			{
				Window* window = ::GetMainWindow();
				::ShowWindow(window->handle, SW_NORMAL);
			}
			void Show()
			{
				Window* window = ::GetMainWindow();
				::ShowWindow(window->handle, SW_SHOW);
				window->base.visible = true;
			}
			void Hide()
			{
				Window* window = ::GetMainWindow();
				::ShowWindow(window->handle, SW_HIDE);
				window->base.visible = false;
			}
			bool IsVisible()
			{
				Window* window = ::GetMainWindow();
				return window->base.visible;
			}
			void SetTitle(const char* title)
			{
				Window* window = ::GetMainWindow();
				::SetWindowTextA(window->handle, title);
			}

		} // namespace window

		// Cursor operations
		namespace cursor {

			void GetPos(float* x, float* y)
			{
				Window* window = ::GetMainWindow();
				RECT winrect;
				POINT pos;
				::GetCursorPos(&pos);
				::GetClientRect(window->handle, &winrect);
				::ScreenToClient(window->handle, &pos);
				if (x)
					*x = static_cast<float>(pos.x);
				if (y)
					*y = static_cast<float>(winrect.bottom - pos.y - 1);
			}
			void SetPos(float x, float y)
			{
				Window* window = ::GetMainWindow();
				RECT winrect;
				POINT pos;
				::GetClientRect(window->handle, &winrect);
				pos.x = static_cast<LONG>(x);
				pos.y = static_cast<LONG>(winrect.bottom - y - 1);
				::ClientToScreen(window->handle, &pos);
				::SetCursorPos((int)pos.x, (int)pos.y);
			}
			void Center()
			{
				Window* window = ::GetMainWindow();
				RECT winrect;
				POINT pos;
				::GetClientRect(window->handle, &winrect);
				pos.x = (winrect.right - winrect.left) / 2;
				pos.y = (winrect.bottom - winrect.top) / 2;
				::ClientToScreen(window->handle, &pos);
				::SetCursorPos((int)(pos.x), (int)(pos.y));
			}
			void Show()
			{
				::ShowCursor(TRUE);
			}
			void Hide()
			{
				::ShowCursor(FALSE);
			}

		} // namespace cursor

		// Clipboard operations
		namespace clipboard {

			void SetText(const char *text)
			{
				size_t len = std::strlen(text);
				HGLOBAL x = ::GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, (len+1)*sizeof(char));
				char* y = (char*)::GlobalLock(x);
				std::strcpy(y, text);
				y[len] = '\0';
				::GlobalUnlock(x);
				::OpenClipboard(NULL);
				::EmptyClipboard();
				::SetClipboardData(CF_TEXT, x);
				::CloseClipboard();
			}
			std::string GetText()
			{
				::OpenClipboard(NULL);
				HANDLE pText = ::GetClipboardData(CF_TEXT);
				::CloseClipboard();
				LPVOID text = ::GlobalLock(pText);
				std::string string = (char*)text;
				::GlobalUnlock(pText);
				return string;
			}

		} // namespace clipboard

	} // namespace platform
} // namespace scythe