#line 2 "togo/impl/gfx/init/sdl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/gfx/init.hpp>
#include <togo/impl/gfx/sdl_common.hpp>
#include <togo/impl/gfx/init/private.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>

namespace togo {

namespace {
	enum {
		INIT_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_VIDEO,
	};
} // anonymous namespace

void gfx::init(
	unsigned context_major,
	unsigned context_minor
) {
	TOGO_SDL_CHECK(SDL_Init(INIT_SYSTEMS) != 0);
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, context_major));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, context_minor));
	if (context_major >= 3) {
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
		TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG));
	}
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to initialize graphics backend: %s", SDL_GetError());
}

void gfx::shutdown() {
	SDL_Quit();
}

} // namespace togo
