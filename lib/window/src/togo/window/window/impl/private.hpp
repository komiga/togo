#line 2 "togo/window/window/impl/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/input/types.hpp>

namespace togo {
namespace window {

void init_impl(
	unsigned context_major,
	unsigned context_minor
);
void shutdown_impl();

bool is_attached_to_input_buffer(Window const* window);
void attach_to_input_buffer(Window* window, InputBuffer& ib);
void detach_from_input_buffer(Window* window);

// Backend
void attach_to_input_buffer_impl(Window* window);
void detach_from_input_buffer_impl(Window* window);
void process_events(InputBuffer& ib);

} // namespace window
} // namespace togo
