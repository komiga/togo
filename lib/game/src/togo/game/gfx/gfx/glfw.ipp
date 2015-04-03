#line 2 "togo/game/gfx/gfx/glfw.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/gfx/private.hpp>
#include <togo/game/gfx/gfx/glfw_common.hpp>

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
