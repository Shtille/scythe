#ifndef __SCYTHE_MOUSE_H__
#define __SCYTHE_MOUSE_H__

namespace scythe {
	
	/**
	 * @brief      This class describes a mouse key.
	 */
	enum class MouseButton {
		kUnknown,
		
		kLeft,
		kRight,
		kMiddle,
		
		kCount
	};

	/**
	 * @brief      This class describes a mouse button state.
	 */
	struct MouseButtonState {
		bool down;		//!< button down flag (button down if true and button up if false)
		bool active;	//!< button active flag
		char pad[2];	//!< a memory pad
	};

	/**
	 * @brief      This class describes a mouse state.
	 * @details    Mouse position is in window coordinates with origin in left bottom corner.
	 */
	struct MouseState {
		static constexpr int kStatesSize = static_cast<int>(MouseButton::kCount);

		MouseState();

		bool IsButtonDown(MouseButton button) const;
		bool IsButtonUp(MouseButton button) const;
		bool IsButtonActive(MouseButton button) const;

		void SetButtonDown(MouseButton button, bool value);
		void SetButtonUp(MouseButton button, bool value);
		void SetButtonActive(MouseButton button, bool value);

		float x;								//!< mouse position in window coordinates in X direction
		float y;								//!< mouse position in window coordinates in Y direction
		float delta_x;							//!< mouse position delta in window coordinates in X direction
		float delta_y;							//!< mouse position delta in window coordinates in Y direction
		MouseButtonState states[kStatesSize];	//!< mouse button to down flag table
	};
	
} // namespace scythe

#endif