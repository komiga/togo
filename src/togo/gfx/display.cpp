#line 2 "togo/gfx/display.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/impl/gfx/display/private.hpp>
#include <togo/input_buffer.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/impl/gfx/display/sdl.ipp>
#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
	#include <togo/impl/gfx/display/glfw.ipp>
#endif

namespace togo {
namespace gfx {

Display::~Display() {
	if (gfx::display::is_attached_to_input_buffer(this)) {
		input_buffer::remove_display(*_input_buffer, this);
	}
}

unsigned display::width(gfx::Display const* display) {
	return display->_width;
}

unsigned display::height(gfx::Display const* display) {
	return display->_height;
}

} // namespace gfx
} // namespace togo
