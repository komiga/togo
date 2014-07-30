#line 2 "togo/impl/gfx/display/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/gfx/types.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/input_types.hpp>

namespace togo {
namespace gfx {
namespace display {

inline bool is_attached_to_input_buffer(
	gfx::Display* const display
) {
	return display->_input_buffer;
}

void attach_to_input_buffer_impl(gfx::Display* display);
void detach_from_input_buffer_impl(gfx::Display* display);

inline void attach_to_input_buffer(gfx::Display* display, InputBuffer& ib) {
	TOGO_ASSERT(
		!gfx::display::is_attached_to_input_buffer(display),
		"display is already attached to an input buffer"
	);
	display->_input_buffer = &ib;
	attach_to_input_buffer_impl(display);
}

inline void detach_from_input_buffer(gfx::Display* display) {
	TOGO_ASSERT(
		gfx::display::is_attached_to_input_buffer(display),
		"display is not attached to an input buffer"
	);
	detach_from_input_buffer_impl(display);
	display->_input_buffer = nullptr;
}

void process_events(InputBuffer& ib);

} // namespace display
} // namespace gfx
} // namespace togo
