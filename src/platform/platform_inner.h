#ifndef __SCYTHE_PLATFORM_INNER_H__
#define __SCYTHE_PLATFORM_INNER_H__

#include <string>

#include <scythe/platform.h>
#ifdef SCYTHE_TARGET_DESKTOP
# include <scythe/window.h>
#endif

namespace scythe {

	// Forward declarations
	class DesktopApplication;
	struct PlatformWindow;

	namespace platform {

		const Window* GetWindowByPlatformOne(const void* platform_window_ptr);
		PlatformWindow* GetPlatformWindow(DesktopApplication* desktop_app);

		bool Initialize();
		void Deinitialize();

		void PollEvents();
		void ChangeDirectoryToResources();

#ifdef SCYTHE_TARGET_DESKTOP

		// Window functions
		namespace window {

			bool Create();
			void Destroy();
			void Terminate();

			void ToggleFullscreen(void);
			bool MakeFullscreen(void);
			void MakeWindowed(void);
			void Center();
			void Resize(int width, int height);
			void Iconify();
			void Restore();
			void Show();
			void Hide();
			bool IsVisible();

			void SetTitle(const char* title);

		} // namespace window

		// Cursor operations
		namespace cursor {

			void GetPos(float* x, float* y); // x=[0,w]; y=[0,h]
			void SetPos(float x, float y); // x=[0,w]; y=[0,h]
			void Center();
			void Show();
			void Hide();

		} // namespace cursor

		// Clipboard operations
		namespace clipboard {

			void SetText(const char *text);
			std::string GetText();

		} // namespace clipboard

#endif // SCYTHE_TARGET_DESKTOP

	} // namespace platform
} // namespace scythe

#endif