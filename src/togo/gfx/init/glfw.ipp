#line 2 "togo/gfx/init/glfw.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/gfx/init.hpp>
#include <togo/gfx/gfx/glfw_common.hpp>
#include <togo/gfx/init/private.hpp>

#include <GLFW/glfw3.h>

namespace togo {

void gfx::init_impl(
	unsigned context_major,
	unsigned context_minor
) {
	TOGO_GLFW_CHECK(glfwInit());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_minor);
	if (context_major >= 3) {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}
	return;

glfw_error:
	TOGO_ASSERT(false, "failed to initialize graphics backend\n");
}

void gfx::shutdown_impl() {
	glfwTerminate();
}

} // namespace togo
