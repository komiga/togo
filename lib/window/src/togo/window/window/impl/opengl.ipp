#line 2 "togo/window/window/impl/opengl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
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

inline static void log_gl_string(StringRef name, GLenum id) {
	auto value = reinterpret_cast<char const*>(glGetString(id));
	TOGO_LOGF("%.*s = %s\n", name.size, name.data, value ? value : "(null)");
}

void init_opengl() {
	if (_globals.opengl_initialized) {
		return;
	}

	TOGO_ASSERT(gladLoadGL(), "failed to initialize OpenGL");

	#define LOG_GL_STRING(name) \
		log_gl_string(#name, name)

	LOG_GL_STRING(GL_VENDOR);
	LOG_GL_STRING(GL_VERSION);
	LOG_GL_STRING(GL_SHADING_LANGUAGE_VERSION);
	LOG_GL_STRING(GL_RENDERER);

	#undef LOG_GL_STRING

	#if defined(TOGO_DEBUG)
	{GLint num = 0;
	TOGO_GLCE_X(glGetIntegerv(GL_NUM_EXTENSIONS, &num));
	TOGO_LOGF("OpenGL extensions (%d):\n", num);
	for (signed i = 0; i < num; ++i) {
		auto value = glGetStringi(GL_EXTENSIONS, i);
		if (value) {
			TOGO_LOGF("    %s\n", value);
		}
	}}
	#endif

	_globals.opengl_initialized = true;
}

} // namespace window
} // namespace togo
