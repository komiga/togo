#line 2 "togo/impl/gfx/init/sdl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/impl/gfx/sdl_common.hpp>
#include <togo/gfx/init.hpp>

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
	TOGO_ASSERT(!_gfx_globals.initialized, "graphics backend has already been initialized");

	TOGO_ASSERT(
		context_major >= 3 && context_minor >= 2,
		"OpenGL context version below 3.2 is not supported"
	);
	TOGO_SDL_CHECK(SDL_Init(INIT_SYSTEMS) != 0);
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, context_major));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, context_minor));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
	TOGO_SDL_CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG));

	_gfx_globals.context_major = context_major;
	_gfx_globals.context_minor = context_minor;
	_gfx_globals.initialized = true;
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to initialize graphics backend: %s", SDL_GetError());
}

void gfx::shutdown() {
	TOGO_ASSERT(_gfx_globals.initialized, "graphics backend has not been initialized");

	SDL_Quit();
	_gfx_globals.context_major = 0;
	_gfx_globals.context_minor = 0;
	_gfx_globals.initialized = false;
}

} // namespace togo
