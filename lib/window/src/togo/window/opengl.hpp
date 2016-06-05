#line 2 "togo/window/opengl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief OpenGL includes.
*/

#pragma once

#include <togo/window/config.hpp>

#if !defined(TOGO_WINDOW_BACKEND_SUPPORTS_OPENGL)
	#error "OpenGL is not supported by the lib/window backend"
#endif

#include <glad/glad.h>

namespace togo {
namespace window {

char const* gl_get_error();

#define TOGO_GLCE_LOG() \
	do { while (char const* gl_error__ = window::gl_get_error()) { \
		TOGO_LOG_ERRORF("OpenGL error: %s\n", gl_error__); \
	} } while (false)

#define TOGO_GLCE(expr_) do { (expr_); TOGO_GLCE_LOG(); } while (false)

void init_opengl();

bool set_opengl_debug_mode(bool active);

} // namespace window
} // namespace togo
