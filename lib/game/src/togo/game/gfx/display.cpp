#line 2 "togo/game/gfx/display.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/math/types.hpp>
#include <togo/game/gfx/display.hpp>
#include <togo/game/gfx/display/types.hpp>
#include <togo/game/gfx/display/private.hpp>
#include <togo/game/input/input_buffer.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/game/gfx/display/sdl.ipp>
#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
	#include <togo/game/gfx/display/glfw.ipp>
#endif

namespace togo {
namespace game {
namespace gfx {

Display::~Display() {
	if (gfx::display::is_attached_to_input_buffer(this)) {
		input_buffer::remove_display(*_input_buffer, this);
	}
}

/// Size.
UVec2 display::size(gfx::Display const* display) {
	return display->_size;
}

/// Width.
unsigned display::width(gfx::Display const* display) {
	return display->_size.x;
}

/// Height.
unsigned display::height(gfx::Display const* display) {
	return display->_size.y;
}

// private

bool display::is_attached_to_input_buffer(gfx::Display const* display) {
	return display->_input_buffer;
}

void display::attach_to_input_buffer(gfx::Display* display, InputBuffer& ib) {
	TOGO_ASSERT(
		!gfx::display::is_attached_to_input_buffer(display),
		"display is already attached to an input buffer"
	);
	display->_input_buffer = &ib;
	attach_to_input_buffer_impl(display);
}

void display::detach_from_input_buffer(gfx::Display* display) {
	TOGO_ASSERT(
		gfx::display::is_attached_to_input_buffer(display),
		"display is not attached to an input buffer"
	);
	detach_from_input_buffer_impl(display);
	display->_input_buffer = nullptr;
}

} // namespace gfx
} // namespace game
} // namespace togo
