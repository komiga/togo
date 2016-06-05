#line 2 "togo/window/window/impl/opengl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>

#include <togo/window/opengl.hpp>

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
