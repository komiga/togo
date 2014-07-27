#line 2 "togo/impl/gfx/display/glfw.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/memory.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/impl/gfx/glfw_common.hpp>
#include <togo/impl/gfx/display/glfw.hpp>
#include <togo/gfx/display.hpp>

#include <GLFW/glfw3.h>

namespace togo {
namespace gfx {

gfx::Display* display::create(
	char const* title,
	unsigned width,
	unsigned height,
	gfx::DisplayFlags flags,
	gfx::Config const& config,
	gfx::Display* share_with,
	Allocator& allocator
) {
	glfwWindowHint(
		GLFW_DECORATED,
		enum_bool(flags & gfx::DisplayFlags::borderless) ? GL_TRUE : GL_FALSE
	);
	glfwWindowHint(
		GLFW_RESIZABLE,
		enum_bool(flags & gfx::DisplayFlags::resizable) ? GL_TRUE : GL_FALSE
	);

	GLFWwindow* const share_with_handle
		= share_with
		? share_with->_impl.handle
		: nullptr
	;
	glfw_config_setup(config);
	GLFWwindow* const handle = glfwCreateWindow(
		width, height, title, nullptr, share_with_handle
	);
	TOGO_ASSERT(handle, "failed to create display");
	glfwMakeContextCurrent(handle);
	gfx::glew_init();

	return TOGO_CONSTRUCT(
		allocator, gfx::Display, width, height, flags, config, allocator,
		GLFWDisplayImpl{handle}
	);
}

void display::set_title(gfx::Display* display, char const* title) {
	glfwSetWindowTitle(display->_impl.handle, title);
}

void display::set_mouse_lock(gfx::Display* /*display*/, bool /*enable*/) {
	TOGO_LOG_DEBUG("gfx::display::set_mouse_lock() not implemented for GLFW\n");
}

void display::make_current(gfx::Display* display) {
	glfwMakeContextCurrent(display->_impl.handle);
	return;
}

void display::swap_buffers(gfx::Display* display) {
	glfwSwapBuffers(display->_impl.handle);
}

void display::destroy(gfx::Display* display) {
	Allocator& allocator = *display->_allocator;
	glfwDestroyWindow(display->_impl.handle);
	TOGO_DESTROY(allocator, display);
}

} // namespace gfx
} // namespace togo
