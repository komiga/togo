#line 2 "togo/gfx/init.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/gfx/init.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/impl/gfx/common.hpp>
#include <togo/impl/gfx/init/private.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/impl/gfx/init/sdl.ipp>
#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
	#include <togo/impl/gfx/init/glfw.ipp>
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
		context_major >= 2 && context_minor >= 1,
		"OpenGL context version below 2.1 is not supported"
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
