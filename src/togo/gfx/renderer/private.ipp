#line 2 "togo/gfx/renderer/private.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/renderer/private.hpp>

namespace togo {
namespace gfx {

namespace renderer {

#define TOGO_GFX_RENDERER_TEARDOWN_RA_(ra, destroy_func)		\
	if (renderer->ra._num > 0) {								\
		for (auto const& slot : renderer->ra._slots) {			\
			if (slot.id._value == ID_VALUE_NULL) {				\
				renderer::destroy_func(renderer, slot.id);		\
			}													\
		}														\
	}

void teardown_base(gfx::Renderer* const renderer) {
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_vertex_buffers, destroy_vertex_buffer);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_index_buffers, destroy_index_buffer);
	//TOGO_GFX_RENDERER_TEARDOWN_RA_(_textures, destroy_texture);
	//TOGO_GFX_RENDERER_TEARDOWN_RA_(_uniforms, destroy_uniform);
	//TOGO_GFX_RENDERER_TEARDOWN_RA_(_shaders, destroy_shader);
}

} // namespace renderer

} // namespace gfx
} // namespace togo
