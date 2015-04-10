#line 2 "togo/game/gfx/renderer/private.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/generator.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/gfx/renderer/private.hpp>

namespace togo {
namespace game {
namespace gfx {

void renderer::init_base(gfx::Renderer* const renderer) {
	// Register standard generators
	gfx::renderer::register_generator_def(renderer, gfx::generator::clear);
}

#define TOGO_GFX_RENDERER_TEARDOWN_RA_(ra, func_destroy)		\
	if (renderer->ra._num > 0) {								\
		for (auto const& slot : renderer->ra._slots) {			\
			if (slot.id._value != ID_VALUE_NULL) {				\
				renderer::func_destroy(renderer, slot.id);		\
			}													\
		}														\
	}

void renderer::teardown_base(gfx::Renderer* const renderer) {
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_buffers, destroy_buffer);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_buffer_bindings, destroy_buffer_binding);
	//TOGO_GFX_RENDERER_TEARDOWN_RA_(_textures, destroy_texture);
	//TOGO_GFX_RENDERER_TEARDOWN_RA_(_uniforms, destroy_uniform);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_shaders, destroy_shader);
	for (auto& pb_name_hash : renderer->_fixed_param_blocks) {
		pb_name_hash = gfx::PB_NAME_NULL;
	}
	renderer->_num_active_draw_param_blocks = 0;
	for (auto& entry : renderer->_generators) {
		if (entry.value.func_destroy) {
			entry.value.func_destroy(entry.value, renderer);
		}
	}
	hash_map::clear(renderer->_generators);
}

void renderer::configure_base(
	gfx::Renderer* const renderer,
	gfx::RenderConfig const& config
) {
	for (auto const& pipe : config.pipes) {
	for (auto const& layer : pipe.layers) {
	for (auto const& gen_unit : layer.layout) {
		TOGO_ASSERTF(
			renderer::find_generator_def(renderer, gen_unit.name_hash),
			"generator unit %08x is not registered",
			gen_unit.name_hash
		);
	}}}
	std::memcpy(&renderer->_config, &config, sizeof(gfx::RenderConfig));
}

} // namespace gfx
} // namespace game
} // namespace togo
