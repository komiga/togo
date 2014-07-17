#line 2 "togo/impl/gfx/context/sdl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>

#include <SDL2/SDL_video.h>

namespace togo {
namespace gfx {

struct SDLContextImpl {
	SDL_GLContext handle;
};

using ContextImpl = SDLContextImpl;

} // namespace gfx
} // namespace togo
