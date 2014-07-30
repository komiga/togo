#line 2 "togo/input_buffer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file input_buffer.hpp
@brief InputBuffer interface.
@ingroup input
*/

#pragma once

#include <togo/config.hpp>
#include <togo/gfx/types.hpp>
#include <togo/input_types.hpp>
#include <togo/object_buffer.hpp>

namespace togo {

/// Construct with allocator for storage.
inline InputBuffer::InputBuffer(
	Allocator& allocator,
	u32 const init_capacity
)
	: _displays()
	, _num_displays(0)
	, _buffer(allocator, init_capacity)
{}

namespace input_buffer {

/**
	@addtogroup input
	@{
*/

/// Add a display.
///
/// If the display is already owned by an input buffer, an assertion
/// will fail. If the input buffer has no space for more displays, an
/// assertion will fail.
void add_display(InputBuffer& ib, gfx::Display* display);

/// Remove a display.
///
/// If the display is not owned by the input buffer, an assertion
/// will fail.
void remove_display(InputBuffer& ib, gfx::Display* display);

/// Poll events.
///
/// Returns true if an event was fetched.
bool poll(InputBuffer& ib, InputEventType& type, InputEvent const*& event);

/** @} */ // end of doc-group input

} // namespace input_buffer
} // namespace togo
