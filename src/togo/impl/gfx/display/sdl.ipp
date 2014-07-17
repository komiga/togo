#line 2 "togo/impl/gfx/display/sdl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/memory.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/impl/gfx/sdl_common.hpp>
#include <togo/gfx/display.hpp>

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>

namespace togo {
namespace gfx {

gfx::Display* display::create(
	char const* title,
	unsigned width,
	unsigned height,
	gfx::DisplayFlags flags,
	gfx::Config const& config,
	Allocator& allocator
) {
	int x;
	int y;
	if (enum_bool(flags & gfx::DisplayFlags::centered)) {
		x = y = SDL_WINDOWPOS_CENTERED;
	} else {
		x = y = SDL_WINDOWPOS_UNDEFINED;
	}

	unsigned sdl_flags
		= SDL_WINDOW_INPUT_GRABBED
		| SDL_WINDOW_ALLOW_HIGHDPI
		| SDL_WINDOW_OPENGL
	;
	if (enum_bool(flags & gfx::DisplayFlags::borderless)) {
		sdl_flags |= SDL_WINDOW_BORDERLESS;
	}
	if (enum_bool(flags & gfx::DisplayFlags::fullscreen)) {
		sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (enum_bool(flags & gfx::DisplayFlags::resizable)) {
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	}

	sdl_config_setup(config);
	SDL_Window* const handle = SDL_CreateWindow(
		title, x, y, width, height, sdl_flags
	);
	TOGO_SDL_CHECK(!handle);

	return TOGO_CONSTRUCT(
		allocator, gfx::Display, width, height, flags, config, allocator,
		SDLDisplayImpl{handle}
	);

sdl_error:
	TOGO_ASSERTF(false, "failed to create display: %s\n", SDL_GetError());
}

void display::set_title(gfx::Display* display, char const* title) {
	SDL_SetWindowTitle(display->_impl.handle, title);
}

void display::set_mouse_lock(gfx::Display* display, bool enable) {
	SDL_SetWindowGrab(display->_impl.handle, enable ? SDL_TRUE : SDL_FALSE);
}

void display::make_current(gfx::Display* display, gfx::Context* context) {
	TOGO_SDL_CHECK(SDL_GL_MakeCurrent(display->_impl.handle, context->_impl.handle));
	return;

sdl_error:
	TOGO_ASSERTF(false, "failed to make window current: %s\n", SDL_GetError());
}

void display::swap_buffers(gfx::Display* display) {
	if (enum_bool(display->_config.flags & gfx::ConfigFlags::double_buffered)) {
		SDL_GL_SwapWindow(display->_impl.handle);
	}
}

void display::destroy(gfx::Display* display) {
	Allocator& allocator = *display->_allocator;
	SDL_DestroyWindow(display->_impl.handle);
	TOGO_DESTROY(allocator, display);
}

} // namespace gfx
} // namespace togo
