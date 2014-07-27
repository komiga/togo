#line 2 "togo/impl/gfx/sdl_common.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/impl/gfx/common.hpp>

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>

#define TOGO_SDL_CHECK(expr) \
	if ((expr)) { goto sdl_error; }

namespace togo {
namespace gfx {

inline void sdl_config_setup(gfx::Config const& config) {
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(
		SDL_GL_DOUBLEBUFFER,
		enum_bool(config.flags & gfx::ConfigFlags::double_buffered) ? 1 : 0
	));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_RED_SIZE, config.color_bits.red));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, config.color_bits.green));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, config.color_bits.blue));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, config.color_bits.alpha));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config.depth_bits));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, config.stencil_bits));

	if (config.msaa_num_buffers == 0 || config.msaa_num_samples == 0) {
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0));
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0));
	} else {
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, config.msaa_num_buffers));
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.msaa_num_samples));
	}
	return;

sdl_error:
	// NB: Keep context memory around for debug
	TOGO_ASSERTF(false, "failed to set OpenGL attribute: %s", SDL_GetError());
}

} // namespace gfx
} // namespace togo
