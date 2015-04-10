#line 2 "togo/game/gfx/gfx/opengl_common.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/log/log.hpp>

#include <GL/glew.h>

namespace togo {
namespace game {
namespace gfx {

// Defined in gfx/renderer/opengl
char const* gl_get_error();

#define TOGO_GLCE() \
	do { while (char const* gl_error__ = gfx::gl_get_error()) { \
		TOGO_LOG_ERRORF("OpenGL error: %s\n", gl_error__); \
	} } while (false)

#define TOGO_GLCE_X(expr_) do { (expr_); TOGO_GLCE(); } while (false)

} // namespace gfx
} // namespace game
} // namespace togo
