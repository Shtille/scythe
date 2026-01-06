#include "../platform_inner.h"

#include <cstring>
#include <Windowsx.h> // for GET_X_LPARAM

#include <scythe/desktop_application.h>

#include "platform_window.h"

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

static constexpr char* kApplicationClassName = "scythe";

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
	scythe::PlatformWindow* window = scythe::platform::GetPlatformWindow(desktop_app);
	scythe::KeyboardState& keyboard_state = desktop_app->GetKeyboardState();
	scythe::MouseState& mouse_state = desktop_app->GetMouseState();
	scythe::KeyboardController* keyboard_controller = desktop_app->GetKeyboardController();
	scythe::MouseController* mouse_controller = desktop_app->GetMouseController();

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
		return 0;
	case WM_KILLFOCUS: // Focus is lost, so minimize the window
		scythe::platform::window::MakeWindowed();
		return 0;

	case WM_SIZE:	// Size Action Has Taken Place
		switch (wParam)
		{
		case SIZE_MINIMIZED:
			window->base.visible = false;
			return 0;

		case SIZE_MAXIMIZED:
			window->base.visible = true;
			return 0;

		case SIZE_RESTORED:
		{
			int width = static_cast<int>(LOWORD(lParam));
			int height = static_cast<int>(HIWORD(lParam));
			window->base.visible = true;
			window->base.width = width;
			window->base.height = height;
			window->base.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
			desktop_app->OnSize(width, height);
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

static inline scythe::PlatformWindow* GetPlatformWindow()
{
	scythe::DesktopApplication* desktop_app = scythe::DesktopApplication::GetInstance();
	return scythe::platform::GetPlatformWindow(desktop_app);
}
static inline void FillWindowParameters(scythe::DesktopApplication* app, scythe::PlatformWindow* window)
{
	window->base.width = app->GetInitialWidth();
	window->base.height = app->GetInitialHeight();
	window->base.aspect_ratio = static_cast<float>(window->base.width) / static_cast<float>(window->base.height);
	window->base.visible = false;
	window->base.fullscreen = app->GetInitialFullscreen();
	window->old_mouse_position = { 0 };
}
static void AdjustVideoSettings()
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::PlatformWindow* window = scythe::platform::GetPlatformWindow(app);

	window->current_state.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	window->current_state.ex_style = WS_EX_APPWINDOW;

	window->current_state.rect.left = 0;
	window->current_state.rect.top = 0;
	window->current_state.rect.right = window->base.width;
	window->current_state.rect.bottom = window->base.height;

	bool windowed = true;
	if (window->base.fullscreen)	// Fullscreen Requested, Try Changing Video Modes
	{
		if (!scythe::platform::window::MakeFullscreen())
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			::MessageBoxA(HWND_DESKTOP, TEXT("Mode Switch Failed.\nRunning In Windowed Mode."), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		}
		else // Otherwise (If Fullscreen Mode Was Successful)
		{
			windowed = false;
			window->current_state.style |= WS_POPUP;
			window->current_state.ex_style |= WS_EX_TOPMOST;
		}
	}
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
static void RestoreVideoSettings()
{
	scythe::DesktopApplication* desktop_app = scythe::DesktopApplication::GetInstance();
	scythe::PlatformWindow* window = scythe::platform::GetPlatformWindow(desktop_app);

	if (window->base.fullscreen)
	{
		::ChangeDisplaySettingsA(NULL, 0);
	}
}

namespace scythe {
	namespace platform {

		const Window* GetWindowByPlatformOne(const void* platform_window_ptr)
		{
			const PlatformWindow* platform_window = reinterpret_cast<const PlatformWindow*>(platform_window_ptr);
			return &platform_window->base;
		}
		PlatformWindow* GetPlatformWindow(DesktopApplication* desktop_app)
		{
			return reinterpret_cast<PlatformWindow*>(desktop_app->platform_window_);
		}

		bool Initialize()
		{
			// Skip any upcoming error
			::GetLastError();

			//	Some dual core systems have a problem where the different CPUs return different
			//	QueryPerformanceCounter values. So when this thread is schedule on the other CPU
			//	in a later frame, we could even get a negative frameTime. To solve this we force
			//	the main thread to always run on CPU 0.
			::SetThreadAffinityMask(::GetCurrentThread(), 1);

			// Create platform window and store its value in application instance
			DesktopApplication* desktop_app = DesktopApplication::GetInstance();
			PlatformWindow* window = new PlatformWindow();
			if (!window) return false;
			desktop_app->platform_window_ = reinterpret_cast<void*>(window);
			::FillWindowParameters(desktop_app, window);

			// This is the way to change window icon manually:
			window->icon = ::LoadIcon(NULL, IDI_APPLICATION);
			if (window->icon == NULL)
			{
				::MessageBoxA(HWND_DESKTOP, TEXT("Icon loading failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
				return false;
			}

			window->instance = GetModuleHandle(NULL);

			// Register A Window Class
			WNDCLASSEXA window_class;
			::ZeroMemory(&window_class, sizeof(WNDCLASSEX));
			window_class.cbSize = sizeof(WNDCLASSEX);
			window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			window_class.lpfnWndProc = (WNDPROC)(WindowProc);
			window_class.hInstance = window->instance;
			window_class.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
			window_class.hCursor = ::LoadCursor(NULL, IDC_ARROW);
			window_class.hIcon = window->icon;
			window_class.hIconSm = window->icon;
			window_class.lpszClassName = kApplicationClassName;
			if (::RegisterClassExA(&window_class) == 0)
			{
				::DestroyIcon(window->icon);
				::MessageBox(HWND_DESKTOP, TEXT("RegisterClassEx Failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
				return false;
			}

			::AdjustVideoSettings();
			return true;
		}
		void Deinitialize()
		{
			::RestoreVideoSettings();

			DesktopApplication* desktop_app = DesktopApplication::GetInstance();
			PlatformWindow* window = scythe::platform::GetPlatformWindow(desktop_app);

			::UnregisterClassA(kApplicationClassName, window->instance);
			::DestroyIcon(window->icon);

			// Destroy platform window
			delete window;
			desktop_app->platform_window_ = nullptr;
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
				PlatformWindow* window = scythe::platform::GetPlatformWindow(app);

				// Create Window
				window->hwnd = ::CreateWindowExA(window->current_state.ex_style,			// Extended Style
					kApplicationClassName,													// Class Name
					app->GetTitle(),														// Window Title
					window->current_state.style,											// Window Style
					0, 0,																	// Window X,Y Position
					window->current_state.rect.right - window->current_state.rect.left,		// Window Width
					window->current_state.rect.bottom - window->current_state.rect.top,		// Window Height
					HWND_DESKTOP,															// Desktop Is Window's Parent
					0,																		// No Menu
					window->instance,														// Pass The Window Instance
					NULL);																	// pointer to window class

				return window->hwnd != NULL;
			}
			void Destroy()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				::DestroyWindow(window->hwnd);
			}
			void Terminate()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				::PostMessage(window->hwnd, WM_CLOSE, 0, 0);
			}

			void ToggleFullscreen(void)
			{
				PlatformWindow* window = ::GetPlatformWindow();
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
			bool MakeFullscreen(void)
			{
				DesktopApplication* app = DesktopApplication::GetInstance();
				PlatformWindow* window = scythe::platform::GetPlatformWindow(app);

				if (window->base.fullscreen)
					return true;

				int width = window->base.width;
				int height = window->base.height;
				int color_bits = app->GetColorBits();

				// Store current window state:
				window->old_state.rect = window->current_state.rect;
				window->old_state.style = window->current_state.style;
				window->old_state.ex_style = window->current_state.ex_style;
				
			    DEVMODE dmScreenSettings;
				ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
				dmScreenSettings.dmSize = sizeof(DEVMODE);
				dmScreenSettings.dmPelsWidth = width;
				dmScreenSettings.dmPelsHeight = height;
				dmScreenSettings.dmBitsPerPel = color_bits;
				dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					// Still windowed
					SetWindowLongPtr(window->hwnd, GWL_STYLE, window->old_state.style);
					SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, window->old_state.ex_style);
					ShowWindow(window->hwnd, SW_NORMAL);
					return false;
				}

				SetWindowLongPtr(window->hwnd, GWL_STYLE, WS_POPUP);
				SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
				MoveWindow(window->hwnd, 0, 0, width, height, FALSE);
				ShowWindow(window->hwnd, SW_NORMAL);
				return true;
			}
			void MakeWindowed(void)
			{
				PlatformWindow* window = ::GetPlatformWindow();

				if (!window->base.fullscreen)
					return;

				::ChangeDisplaySettings(NULL, 0); // restore display settings

				// Restore window state
				::SetWindowLongPtr(window->hwnd, GWL_STYLE, window->old_state.style);
				::SetWindowLongPtr(window->hwnd, GWL_EXSTYLE, window->old_state.ex_style);
				::MoveWindow(window->hwnd, window->old_state.rect.left, window->old_state.rect.top,
					window->old_state.rect.right - window->old_state.rect.left,
					window->old_state.rect.bottom - window->old_state.rect.top, FALSE);
				::ShowWindow(window->hwnd, SW_NORMAL);
			}
			void Center()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				RECT window_rect;
				::GetWindowRect(window->hwnd, &window_rect);
				int screen_width = ::GetSystemMetrics(SM_CXSCREEN);
				int screen_height = ::GetSystemMetrics(SM_CYSCREEN);
				int window_width = window_rect.right - window_rect.left;
				int window_height = window_rect.bottom - window_rect.top;
				window_rect.left = (screen_width - window_width)/2;
				window_rect.top = (screen_height - window_height)/2;
				::MoveWindow(window->hwnd, window_rect.left, window_rect.top, window_width, window_height, TRUE);
			}
			void Resize(int width, int height)
			{
				PlatformWindow* window = ::GetPlatformWindow();
				RECT rect;
				POINT pos;
				pos.x = width;
				pos.y = height;
				::GetWindowRect(window->hwnd, &rect);
				::ClientToScreen(window->hwnd, &pos);
				::MoveWindow(window->hwnd, rect.left, rect.top, pos.x - rect.left, pos.y - rect.top, TRUE);
			}
			void Iconify()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				::ShowWindow(window->hwnd, SW_MINIMIZE);
			}
			void Restore()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				::ShowWindow(window->hwnd, SW_NORMAL);
			}
			void Show()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				::ShowWindow(window->hwnd, SW_SHOW);
				window->base.visible = true;
			}
			void Hide()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				::ShowWindow(window->hwnd, SW_HIDE);
				window->base.visible = false;
			}
			bool IsVisible()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				return window->base.visible;
			}
			void SetTitle(const char* title)
			{
				PlatformWindow* window = ::GetPlatformWindow();
				::SetWindowTextA(window->hwnd, title);
			}

		} // namespace window

		// Cursor operations
		namespace cursor {

			void GetPos(float* x, float* y)
			{
				PlatformWindow* window = ::GetPlatformWindow();
				RECT winrect;
				POINT pos;
				::GetCursorPos(&pos);
				::GetClientRect(window->hwnd, &winrect);
				::ScreenToClient(window->hwnd, &pos);
				if (x)
					*x = static_cast<float>(pos.x);
				if (y)
					*y = static_cast<float>(winrect.bottom - pos.y - 1);
			}
			void SetPos(float x, float y)
			{
				PlatformWindow* window = ::GetPlatformWindow();
				RECT winrect;
				POINT pos;
				::GetClientRect(window->hwnd, &winrect);
				pos.x = static_cast<LONG>(x);
				pos.y = static_cast<LONG>(winrect.bottom - y - 1);
				::ClientToScreen(window->hwnd, &pos);
				::SetCursorPos((int)pos.x, (int)pos.y);
			}
			void Center()
			{
				PlatformWindow* window = ::GetPlatformWindow();
				RECT winrect;
				POINT pos;
				::GetClientRect(window->hwnd, &winrect);
				pos.x = (winrect.right - winrect.left) / 2;
				pos.y = (winrect.bottom - winrect.top) / 2;
				::ClientToScreen(window->hwnd, &pos);
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