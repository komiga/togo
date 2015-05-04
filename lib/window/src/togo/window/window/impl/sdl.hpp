#line 2 "togo/window/window/impl/sdl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>

#include <SDL2/SDL_video.h>

namespace togo {

struct SDLWindowImpl {
	SDL_Window* handle;

#if defined(TOGO_CONFIG_WINDOW_ENABLE_OPENGL)
	SDL_GLContext context;
#else
	void* context;
#endif
};

using WindowImpl = SDLWindowImpl;

#define TOGO_SDL_CHECK(expr) \
	if ((expr)) { goto sdl_error; }

} // namespace togo
