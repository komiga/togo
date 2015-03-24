#line 2 "togo/input/input_buffer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief InputBuffer interface.
@ingroup input
@ingroup input_buffer
*/

#pragma once

#include <togo/config.hpp>
#include <togo/io/object_buffer.hpp>
#include <togo/gfx/types.hpp>
#include <togo/input/types.hpp>

namespace togo {

/// Construct with allocator for storage.
inline InputBuffer::InputBuffer(
	Allocator& allocator,
	u32 const init_capacity
)
	: _num_displays(0)
	, _displays()
	, _buffer(allocator, init_capacity)
{}

namespace input_buffer {

/**
	@addtogroup input_buffer
	@{
*/

/// Number of displays.
inline unsigned num_displays(InputBuffer& ib) {
	return ib._num_displays;
}

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

/// Update input states.
///
/// This should be executed once per frame before polling the input
/// buffer.
void update(InputBuffer& ib);

/// Poll events.
///
/// Returns true if an event was fetched.
///
/// @warning This must be called on the thread that created the
/// displays.
bool poll(InputBuffer& ib, InputEventType& type, InputEvent const*& event);

/** @} */ // end of doc-group input_buffer

} // namespace input_buffer
} // namespace togo
