#include <scythe/mouse.h>

#include <cstring>

namespace scythe {

	MouseState::MouseState()
	{
		// Fill states table
		std::memset(states, 0, kStatesSize * sizeof(MouseButtonState));
	}
	bool MouseState::IsButtonDown(MouseButton button) const
	{
		return states[static_cast<int>(button)].down;
	}
	bool MouseState::IsButtonUp(MouseButton button) const
	{
		return !IsButtonDown(button);
	}
	bool MouseState::IsButtonActive(MouseButton button) const
	{
		return states[static_cast<int>(button)].active;
	}
	void MouseState::SetButtonDown(MouseButton button, bool value)
	{
		states[static_cast<int>(button)].down = value;
	}
	void MouseState::SetButtonUp(MouseButton button, bool value)
	{
		states[static_cast<int>(button)].down = !value;
	}
	void MouseState::SetButtonActive(MouseButton button, bool value)
	{
		states[static_cast<int>(button)].active = value;
	}
	
}