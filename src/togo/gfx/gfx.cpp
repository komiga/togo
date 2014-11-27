#line 2 "togo/gfx/gfx.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/gfx/gfx.hpp>
#include <togo/gfx/gfx/common.hpp>
#include <togo/gfx/gfx/private.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/gfx/gfx/sdl.ipp>
#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
	#include <togo/gfx/gfx/glfw.ipp>
#endif

namespace togo {

namespace gfx {

gfx::Globals _gfx_globals{false, false, 0, 0};

} // namespace gfx

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

void gfx::shutdown() {
	TOGO_ASSERT(_gfx_globals.initialized, "graphics backend has not been initialized");

	gfx::shutdown_impl();
	_gfx_globals.context_major = 0;
	_gfx_globals.context_minor = 0;
	_gfx_globals.initialized = false;
}

} // namespace togo
