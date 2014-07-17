#line 2 "togo/impl/gfx/common.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/assert.hpp>

#include <GL/glew.h>

namespace togo {
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
		GLenum const err = glewInit();
		TOGO_ASSERTF(err == GLEW_OK, "failed to initialize GLEW: %s\n", glewGetErrorString(err));
		_gfx_globals.glew_initialized = true;
	}
}

} // namespace gfx
} // namespace togo
