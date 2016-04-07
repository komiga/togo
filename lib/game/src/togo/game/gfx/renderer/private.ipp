#line 2 "togo/game/gfx/renderer/private.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/io/memory_stream.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
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
	gfx::renderer::register_generator_def(renderer, gfx::generator::fullscreen_pass);
}

#define TOGO_GFX_RENDERER_TEARDOWN_RA_(ra, func_destroy)		\
	if (renderer->ra._num > 0) {								\
		for (auto const& slot : renderer->ra._slots) {			\
			if (slot.id._value != ID_VALUE_NULL) {				\
				gfx::renderer::func_destroy(renderer, slot.id);	\
			}													\
		}														\
	}

void renderer::teardown_base(gfx::Renderer* const renderer) {
	for (auto& entry : renderer->_generators) {
		if (entry.value.func_destroy) {
			entry.value.func_destroy(entry.value, renderer);
		}
	}
	hash_map::clear(renderer->_generators);

	fixed_array::clear(renderer->_shared_rts);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_buffers, destroy_buffer);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_buffer_bindings, destroy_buffer_binding);
	//TOGO_GFX_RENDERER_TEARDOWN_RA_(_textures, destroy_texture);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_render_targets, destroy_render_target);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_framebuffers, destroy_framebuffer);
	//TOGO_GFX_RENDERER_TEARDOWN_RA_(_uniforms, destroy_uniform);
	TOGO_GFX_RENDERER_TEARDOWN_RA_(_shaders, destroy_shader);
	for (auto& pb_name_hash : renderer->_fixed_param_blocks) {
		pb_name_hash = gfx::PB_NAME_NULL;
	}
	renderer->_num_active_draw_param_blocks = 0;
}

void renderer::configure_base(
	gfx::Renderer* const renderer,
	gfx::PackedRenderConfig const& packed_config,
	Endian const endian
) {
	for (auto const& pipe : packed_config.config.pipes) {
	for (auto const& layer : pipe.layers) {
	for (auto const& gen_unit : layer.layout) {
		TOGO_ASSERTF(
			gfx::renderer::find_generator_def(renderer, gen_unit.name_hash),
			"generator unit %08x is not registered",
			gen_unit.name_hash
		);
	}}}

	// Destroy previous shared resources
	for (auto id : renderer->_shared_rts) {
		gfx::renderer::destroy_render_target(renderer, id);
	}

	std::memcpy(&renderer->_config, &packed_config.config, sizeof(gfx::RenderConfig));

	// Create shared resources
	for (auto& resource : renderer->_config.shared_resources) {
		switch (resource.properties & gfx::RenderConfigResource::MASK_TYPE) {
		case gfx::RenderConfigResource::TYPE_RENDER_TARGET:
			fixed_array::push_back(
				renderer->_shared_rts,
				gfx::renderer::create_render_target(renderer, resource.data.render_target)
			);
			break;

		default:
			TOGO_ASSERT(false, "unhandled render config resource type");
		}
	}

	// Reinitialize generator definitions (unload existing unit data)
	for (auto& entry : renderer->_generators) {
		if (entry.value.func_init) {
			entry.value.func_init(entry.value, renderer);
		}
	}

	{// Deserialize generator units
	MemoryReader stream{packed_config.unit_data};
	BinaryInputSerializer ser{stream, endian};
	for (auto& pipe : renderer->_config.pipes) {
	for (auto& layer : pipe.layers) {
	for (auto& gen_unit : layer.layout) {
		gen_unit.data = nullptr;
		auto* gen_def = gfx::renderer::find_generator_def(renderer, gen_unit.name_hash);
		TOGO_ASSERTE(gen_def);
		gen_unit.func_exec = gen_def->func_exec_unit;
		gen_def->func_read_unit(*gen_def, renderer, ser, gen_unit);
		TOGO_ASSERTE(gen_unit.func_exec);
	}}}}
}

} // namespace gfx
} // namespace game
} // namespace togo
