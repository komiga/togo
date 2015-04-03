#line 2 "togo/game/input/input.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/game/gfx/display/types.hpp>
#include <togo/game/input/input.hpp>

namespace togo {

/// Whether key has been pressed for display.
bool input::key_pressed(gfx::Display* display, KeyCode const code) {
	return display->_key_states[static_cast<unsigned>(code)];
}

/// Whether key has been released for display.
bool input::key_released(gfx::Display* display, KeyCode const code) {
	return display->_key_states[
		static_cast<unsigned>(code)
	] & (1 << static_cast<unsigned>(KeyAction::release));
}

/// Whether button has been pressed for display.
bool input::mouse_button_pressed(gfx::Display* display, MouseButton const button) {
	return display->_mouse_button_states[static_cast<unsigned>(button)];
}

/// Whether button has been released for display.
bool input::mouse_button_released(gfx::Display* display, MouseButton const button) {
	return display->_mouse_button_states[
		static_cast<unsigned>(button)
	] & (1 << static_cast<unsigned>(MouseButtonAction::release));
}

/// Mouse position in display.
Vec2 input::mouse_position(gfx::Display* display) {
	return Vec2{
		static_cast<float>(display->_mouse_x),
		static_cast<float>(display->_mouse_y)
	};
}

} // namespace togo
