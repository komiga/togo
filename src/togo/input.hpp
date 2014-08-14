#line 2 "togo/input.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file input.hpp
@brief Input interface.
@ingroup input
*/

#pragma once

#include <togo/config.hpp>
#include <togo/gfx/types.hpp>
#include <togo/input_types.hpp>
#include <togo/math_types.hpp>

namespace togo {
namespace input {

/**
	@addtogroup input
	@{
*/

/// Check if a key has been pressed for a display.
bool key_pressed(gfx::Display* display, KeyCode const code);

/// Check if a key has been released for a display.
bool key_released(gfx::Display* display, KeyCode const code);

/// Check if a mouse button has been pressed for a display.
bool mouse_button_pressed(gfx::Display* display, MouseButton const button);

/// Check if a mouse button has been released for a display.
bool mouse_button_released(gfx::Display* display, MouseButton const button);

/// Get mouse position for a display.
Vec2 mouse_position(gfx::Display* display);

/** @} */ // end of doc-group input

} // namespace input
} // namespace togo
