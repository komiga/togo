#line 2 "togo/game/gfx/display/sdl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>

#include <SDL2/SDL_video.h>

namespace togo {
namespace game {
namespace gfx {

struct SDLDisplayImpl {
	SDL_Window* handle;
	SDL_GLContext context;
};

using DisplayImpl = SDLDisplayImpl;

} // namespace gfx
} // namespace game
} // namespace togo
