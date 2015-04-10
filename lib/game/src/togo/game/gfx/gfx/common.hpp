#line 2 "togo/game/gfx/gfx/common.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/game/gfx/gfx/opengl_common.hpp>

#include <GL/glew.h>

namespace togo {
namespace game {
namespace gfx {

struct Globals {
	bool initialized;
	bool glew_initialized;
	unsigned context_major;
	unsigned context_minor;
};

extern gfx::Globals _gfx_globals;

inline void glew_init() {
	if (!_gfx_globals.glew_initialized) {
		glewExperimental = GL_TRUE;
		GLenum const err = glewInit();
		TOGO_ASSERTF(
			err == GLEW_OK,
			"failed to initialize GLEW: %s\n",
			glewGetErrorString(err)
		);
		TOGO_GLCE();
		_gfx_globals.glew_initialized = true;
	}
}

} // namespace gfx
} // namespace game
} // namespace togo
