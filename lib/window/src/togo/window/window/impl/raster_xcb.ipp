#line 2 "togo/window/window/impl/raster_xcb.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/io/object_buffer.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/window/impl/private.hpp>
#include <togo/window/window/impl/raster_xcb.hpp>
#include <togo/window/input/types.hpp>

namespace togo {

void window::init_impl() {
}

void window::shutdown_impl() {
}

Window* window::create_raster(
	StringRef /*title*/,
	UVec2 /*size*/,
	WindowFlags /*flags*/,
	Allocator& /*allocator*/
) {
	TOGO_ASSERT(false, "TODO");
	return nullptr;
}

void window::destroy(Window* window) {
	Allocator& allocator = *window->_allocator;
	TOGO_DESTROY(allocator, window);
}

void window::set_title(Window* /*window*/, StringRef /*title*/) {
	// TODO
}

void window::set_mouse_lock(Window* /*window*/, bool /*enable*/) {
	// TODO
}

// private

void window::attach_to_input_buffer_impl(Window* /*window*/) {
	// TODO
}

void window::detach_from_input_buffer_impl(Window* /*window*/) {
	// TODO
}

void window::process_events(InputBuffer& /*ib*/) {
	// TODO
}

} // namespace togo
