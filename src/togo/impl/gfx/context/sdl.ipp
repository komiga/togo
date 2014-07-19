#line 2 "togo/impl/gfx/context/sdl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/memory.hpp>
#include <togo/impl/gfx/common.hpp>
#include <togo/impl/gfx/sdl_common.hpp>
#include <togo/impl/gfx/context/sdl.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/gfx/context.hpp>

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>

namespace togo {
namespace gfx {

gfx::Context* context::create(
	gfx::Display* display,
	gfx::ContextFlags flags,
	Allocator& allocator
) {
	SDL_GLContext handle = nullptr;

	TOGO_SDL_CHECK(SDL_GL_SetAttribute(
		SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
		enum_bool(flags & gfx::ContextFlags::shared) ? 1 : 0
	));
	sdl_config_setup(display->_config);
	handle = SDL_GL_CreateContext(display->_impl.handle);
	TOGO_SDL_CHECK(!handle);
	gfx::glew_init();

	return TOGO_CONSTRUCT(
		allocator, gfx::Context, flags, allocator,
		SDLContextImpl{handle}
	);

sdl_error:
	// NB: Keep context memory around for debug
	TOGO_ASSERTF(false, "failed to create context: %s\n", SDL_GetError());
}

void context::destroy(gfx::Context* context) {
	Allocator& allocator = *context->_allocator;
	SDL_GL_DeleteContext(context->_impl.handle);
	TOGO_DESTROY(allocator, context);
}

} // namespace gfx
} // namespace togo
