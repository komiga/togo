#line 2 "togo/input.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/input.hpp>

namespace togo {

bool input::key_pressed(gfx::Display* display, KeyCode const code) {
	return display->_key_states[static_cast<unsigned>(code)];
}

bool input::key_released(gfx::Display* display, KeyCode const code) {
	return display->_key_states[
		static_cast<unsigned>(code)
	] & (1 << static_cast<unsigned>(KeyAction::release));
}

bool input::mouse_button_pressed(gfx::Display* display, MouseButton const button) {
	return display->_mouse_button_states[static_cast<unsigned>(button)];
}

bool input::mouse_button_released(gfx::Display* display, MouseButton const button) {
	return display->_mouse_button_states[
		static_cast<unsigned>(button)
	] & (1 << static_cast<unsigned>(MouseButtonAction::release));
}

Vec2 input::mouse_position(gfx::Display* display) {
	return Vec2{
		static_cast<float>(display->_mouse_x),
		static_cast<float>(display->_mouse_y)
	};
}

} // namespace togo
