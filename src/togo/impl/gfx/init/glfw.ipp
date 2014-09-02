#line 2 "togo/impl/gfx/init/glfw.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/impl/gfx/glfw_common.hpp>
#include <togo/gfx/init.hpp>

#include <GLFW/glfw3.h>

namespace togo {

void gfx::init(
	unsigned context_major,
	unsigned context_minor
) {
	TOGO_ASSERT(!_gfx_globals.initialized, "graphics backend has already been initialized");

	TOGO_ASSERT(
		context_major >= 2 && context_minor >= 1,
		"OpenGL context version below 2.1 is not supported"
	);
	TOGO_GLFW_CHECK(glfwInit());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_minor);
	if (context_major >= 3) {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}

	_gfx_globals.context_major = context_major;
	_gfx_globals.context_minor = context_minor;
	_gfx_globals.initialized = true;
	return;

glfw_error:
	TOGO_ASSERT(false, "failed to initialize graphics backend\n");
}

void gfx::shutdown() {
	TOGO_ASSERT(_gfx_globals.initialized, "graphics backend has not been initialized");

	glfwTerminate();
	_gfx_globals.context_major = 0;
	_gfx_globals.context_minor = 0;
	_gfx_globals.initialized = false;
}

} // namespace togo
