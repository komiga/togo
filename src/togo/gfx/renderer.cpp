#line 2 "togo/gfx/renderer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/algorithm/sort.hpp>
#include <togo/gfx/command.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/renderer/private.hpp>
#include <togo/gfx/renderer/private.ipp>

#if (TOGO_CONFIG_RENDERER == TOGO_RENDERER_OPENGL)
	#include <togo/gfx/renderer/opengl.ipp>
#endif

#include <cstring>

namespace togo {
namespace gfx {

Renderer::Renderer(
	Allocator& allocator,
	gfx::RendererImpl&& impl
)
	: _allocator(&allocator)
	, _impl(impl)
	, _shader_stage(allocator)
	, _num_active_draw_param_blocks(0)
	, _fixed_param_blocks()
	, _generators(allocator)
	, _config()
	, _buffers()
	, _buffer_bindings()
	, _textures()
	, _uniforms()
	, _shaders()
	, _nodes()
{}

void renderer::register_generator_def(
	gfx::Renderer* const renderer,
	gfx::GeneratorDef const& def
) {
	TOGO_ASSERT(
		def.func_read_unit,
		"func_read must be assigned in generator definition"
	);
	TOGO_ASSERTF(
		!hash_map::has(renderer->_generators, def.name_hash),
		"generator %08x has already been registered",
		def.name_hash
	);
	hash_map::push(renderer->_generators, def.name_hash, def);
}

gfx::GeneratorDef const* renderer::find_generator_def(
	gfx::Renderer const* const renderer,
	gfx::GeneratorNameHash const name_hash
) {
	return hash_map::find(renderer->_generators, name_hash);
}

namespace {
struct CmdKeyKeyFunc {
	inline u64 operator()(gfx::CmdKey const& key) const noexcept {
		return key.key;
	}
};
} // anonymous namespace

void renderer::render_objects(
	gfx::Renderer* const renderer,
	unsigned const num_objects,
	gfx::RenderObject const* const objects,
	gfx::Camera const& /*camera*/,
	gfx::ViewportNameHash const viewport_name_hash
) {
	auto const& rc = renderer->_config;
	gfx::Viewport const* viewport = nullptr;
	for (auto& it : rc.viewports) {
		if (it.name_hash == viewport_name_hash) {
			viewport = &it;
			break;
		}
	}
	TOGO_ASSERTE(viewport);
	for (auto& node : renderer->_nodes) {
		node.num_commands = 0;
		node.buffer_size = 0;
	}

	// TODO: Distribute across nodes
	auto& pipe = rc.pipes[viewport->pipe];
	for (auto& layer : pipe.layers) {
		auto& node = renderer->_nodes[0];
		node.sequence = layer.seq_base;
	for (auto& gen_unit : layer.layout) {
		gen_unit.func_exec(gen_unit, node, objects, objects + num_objects);
		++node.sequence;
	}}

	unsigned num_commands = 0;

	{// Join keys from all nodes
	auto* keys = renderer->_joined_keys_a;
	for (auto const& node : renderer->_nodes) {
		if (node.num_commands > 0) {
			std::memcpy(keys, node.keys, sizeof(gfx::CmdKey) * node.num_commands);
			keys += node.num_commands;
			num_commands += node.num_commands;
		}
	}}

	auto* keys = renderer->_joined_keys_a;
	{// Sort commands
	auto* keys_swap = renderer->_joined_keys_b;
	sort_radix_generic<gfx::CmdKey, u64, u32>(
		keys, keys_swap,
		num_commands,
		CmdKeyKeyFunc{}
	);
	}

	{// Execute commands
	gfx::CmdType type;
	void const* data_untyped;
	for (auto const& key : array_ref(num_commands, keys)) {
		type = *static_cast<gfx::CmdType const*>(key.data);
		data_untyped = pointer_add(key.data, sizeof(gfx::CmdType));
		switch (type) {
		case gfx::CmdType::Callback:
			// TODO
			break;

		case gfx::CmdType::ClearBackbuffer:
			gfx::renderer::clear_backbuffer(renderer);
			break;

		case gfx::CmdType::RenderBuffers: {
			auto* d = static_cast<gfx::CmdRenderBuffers const*>(data_untyped);
			gfx::renderer::render_buffers(
				renderer,
				d->shader_id,
				d->num_draw_param_blocks,
				d->draw_param_blocks,
				d->num_buffers,
				d->buffers
			);
		}	break;
		}
	}}
}

} // namespace gfx
} // namespace togo
