#line 2 "togo/gfx/init.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/impl/gfx/types.hpp>
#include <togo/impl/gfx/common.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/impl/gfx/init/sdl.ipp>
#endif

namespace togo {
namespace gfx {

gfx::Globals _gfx_globals{false, false, 0, 0};

} // namespace gfx
} // namespace togo
