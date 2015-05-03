#line 2 "togo/window/window/impl/opengl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/window/impl/opengl.hpp>

namespace togo {
namespace window {

char const* gl_get_error() {
	#define TOGO_RETURN_ERROR(err_name_) \
		case err_name_: return #err_name_;

	GLenum const error_code = glGetError();
	switch (error_code) {
	case GL_NO_ERROR: return nullptr;
	TOGO_RETURN_ERROR(GL_INVALID_ENUM)
	TOGO_RETURN_ERROR(GL_INVALID_VALUE)
	TOGO_RETURN_ERROR(GL_INVALID_OPERATION)
	TOGO_RETURN_ERROR(GL_INVALID_FRAMEBUFFER_OPERATION)
	TOGO_RETURN_ERROR(GL_OUT_OF_MEMORY)
	TOGO_RETURN_ERROR(GL_STACK_OVERFLOW)
	TOGO_RETURN_ERROR(GL_STACK_UNDERFLOW)
	default: return "UNKNOWN";
	}
	#undef TOGO_RETURN_ERROR
}

void glew_init() {
	if (!_globals.glew_initialized) {
		glewExperimental = GL_TRUE;
		GLenum const err = glewInit();
		TOGO_ASSERTF(
			err == GLEW_OK,
			"failed to initialize GLEW: %s\n",
			glewGetErrorString(err)
		);
		TOGO_GLCE();
		_globals.glew_initialized = true;
	}
}

} // namespace window
} // namespace togo
