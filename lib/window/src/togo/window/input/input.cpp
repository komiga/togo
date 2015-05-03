#line 2 "togo/window/input/input.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/input/input.hpp>

namespace togo {

/// Whether key has been pressed for window.
bool input::key_pressed(Window* window, KeyCode const code) {
	return window->_key_states[static_cast<unsigned>(code)];
}

/// Whether key has been released for window.
bool input::key_released(Window* window, KeyCode const code) {
	return window->_key_states[
		static_cast<unsigned>(code)
	] & (1 << static_cast<unsigned>(KeyAction::release));
}

/// Whether button has been pressed for window.
bool input::mouse_button_pressed(Window* window, MouseButton const button) {
	return window->_mouse_button_states[static_cast<unsigned>(button)];
}

/// Whether button has been released for window.
bool input::mouse_button_released(Window* window, MouseButton const button) {
	return window->_mouse_button_states[
		static_cast<unsigned>(button)
	] & (1 << static_cast<unsigned>(MouseButtonAction::release));
}

/// Mouse position in window.
Vec2 input::mouse_position(Window* window) {
	return Vec2{
		static_cast<float>(window->_mouse_x),
		static_cast<float>(window->_mouse_y)
	};
}

} // namespace togo
