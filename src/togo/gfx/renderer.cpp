#line 2 "togo/gfx/renderer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/impl/gfx/renderer/types.hpp>
#include <togo/impl/gfx/renderer/private.hpp>
#include <togo/impl/gfx/renderer/private.ipp>

#if (TOGO_CONFIG_RENDERER == TOGO_RENDERER_OPENGL)
	#include <togo/impl/gfx/renderer/opengl.ipp>
#endif
