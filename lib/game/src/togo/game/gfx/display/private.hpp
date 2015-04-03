#line 2 "togo/game/gfx/display/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/input/types.hpp>

namespace togo {
namespace gfx {
namespace display {

bool is_attached_to_input_buffer(gfx::Display const* display);
void attach_to_input_buffer(gfx::Display* display, InputBuffer& ib);
void detach_from_input_buffer(gfx::Display* display);

// Backend
void attach_to_input_buffer_impl(gfx::Display* display);
void detach_from_input_buffer_impl(gfx::Display* display);
void process_events(InputBuffer& ib);

} // namespace display
} // namespace gfx
} // namespace togo
