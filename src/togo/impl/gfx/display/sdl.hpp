#line 2 "togo/impl/gfx/display/sdl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>

#include <SDL2/SDL_video.h>

namespace togo {
namespace gfx {

struct SDLDisplayImpl {
	SDL_Window* handle;
};

using DisplayImpl = SDLDisplayImpl;

} // namespace gfx
} // namespace togo
