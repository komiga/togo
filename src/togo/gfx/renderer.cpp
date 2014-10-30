#line 2 "togo/gfx/renderer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/renderer/private.hpp>
#include <togo/gfx/renderer/private.ipp>

#if (TOGO_CONFIG_RENDERER == TOGO_RENDERER_OPENGL)
	#include <togo/gfx/renderer/opengl.ipp>
#endif
