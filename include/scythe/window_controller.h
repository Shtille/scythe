#ifndef __SCYTHE_WINDOW_CONTROLLER_H__
#define __SCYTHE_WINDOW_CONTROLLER_H__

namespace scythe {

	/**
	 * @brief      Defines handler for window specific messages.
	 */
	class WindowController
	{
	public:
		virtual ~WindowController() = default;

		virtual void OnResize(int width, int height) {}
		virtual void OnMinimized() {}
		virtual void OnMaximized() {}
		virtual void OnGetFocus() {}
		virtual void OnLostFocus() {}
	};

} // namespace scythe

#endif