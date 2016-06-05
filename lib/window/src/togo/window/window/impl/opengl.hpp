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

#define TOGO_GLCE() \
	do { while (char const* gl_error__ = gl_get_error()) { \
		TOGO_LOG_ERRORF("OpenGL error: %s\n", gl_error__); \
	} } while (false)

#define TOGO_GLCE_X(expr_) do { (expr_); TOGO_GLCE(); } while (false)

void init_opengl();

} // namespace window
} // namespace togo
