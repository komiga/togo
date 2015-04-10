#line 2 "togo/game/gfx/gfx.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/gfx/private.hpp>
#include <togo/game/gfx/gfx/common.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/game/gfx/gfx/sdl.ipp>
#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
	#include <togo/game/gfx/gfx/glfw.ipp>
#endif

namespace togo {
namespace game {

namespace gfx {

gfx::Globals _gfx_globals{false, false, 0, 0};

} // namespace gfx

/// Initialize the graphics backend.
///
/// context_major and context_minor are OpenGL context version numbers.
/// If these are less than 3 and 2, respectively, an assertion will fail.
/// The core profile is forced.
void gfx::init(
	unsigned context_major,
	unsigned context_minor
) {
	TOGO_ASSERT(!_gfx_globals.initialized, "graphics backend has already been initialized");

	TOGO_ASSERT(
		context_major >= 3 && context_minor >= 3,
		"OpenGL context version below 3.3 is not supported"
	);
	gfx::init_impl(context_major, context_minor);

	_gfx_globals.context_major = context_major;
	_gfx_globals.context_minor = context_minor;
	_gfx_globals.initialized = true;
	return;
}

/// Shutdown the graphics backend.
void gfx::shutdown() {
	TOGO_ASSERT(_gfx_globals.initialized, "graphics backend has not been initialized");

	gfx::shutdown_impl();
	_gfx_globals.context_major = 0;
	_gfx_globals.context_minor = 0;
	_gfx_globals.initialized = false;
}

} // namespace game
} // namespace togo
