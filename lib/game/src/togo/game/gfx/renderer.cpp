#line 2 "togo/game/gfx/renderer.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/collection/hash_map.hpp>
#include <togo/core/algorithm/sort.hpp>
#include <togo/core/threading/condvar.hpp>
#include <togo/core/threading/mutex.hpp>
#include <togo/core/threading/task_manager.hpp>
#include <togo/game/gfx/display.hpp>
#include <togo/game/gfx/command.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/gfx/renderer/private.hpp>
#include <togo/game/gfx/renderer/private.ipp>

#if (TOGO_CONFIG_RENDERER == TOGO_RENDERER_OPENGL)
	#include <togo/game/gfx/renderer/opengl.ipp>
#endif

#include <cstring>

namespace togo {
namespace game {
namespace gfx {

Renderer::Renderer(
	Allocator& allocator,
	gfx::RendererImpl&& impl
)
	: _allocator(&allocator)
	, _impl(impl)
	, _viewport_size(1, 1)
	, _shader_stage(allocator)
	, _num_active_draw_param_blocks(0)
	, _fixed_param_blocks()
	, _generators(allocator)
	, _frame_mutex()
	, _frame_condvar()
	, _work_data()
	, _shared_rts()
	, _config()
	, _buffers()
	, _buffer_bindings()
	, _textures()
	, _render_targets()
	, _uniforms()
	, _shaders()
	, _nodes()
{}

/// Register generator definition.
///
/// An assertion will fail if the generator definition is malformed or is
/// already registered.
/// An assertion will fail if func_init is assigned and func_destroy is not
/// assigned.
void renderer::register_generator_def(
	gfx::Renderer* const renderer,
	gfx::GeneratorDef const& def
) {
	TOGO_ASSERT(
		def.func_read_unit,
		"func_read_unit must be assigned in generator definition"
	);
	TOGO_ASSERT(
		!def.func_init || def.func_destroy,
		"func_destroy must be assigned in generator definition if func_init is assigned"
	);
	TOGO_ASSERTF(
		!hash_map::has(renderer->_generators, def.name_hash),
		"generator %08x has already been registered",
		def.name_hash
	);

	auto& def_mapped = hash_map::push(renderer->_generators, def.name_hash, def);
	if (def_mapped.func_init) {
		def_mapped.func_init(def_mapped, renderer);
	}
}

/// Find generator definition by name.
gfx::GeneratorDef const* renderer::find_generator_def(
	gfx::Renderer const* const renderer,
	gfx::GeneratorNameHash const name_hash
) {
	return hash_map::find(renderer->_generators, name_hash);
}

/// Get viewport size.
UVec2 renderer::viewport_size(gfx::Renderer* const renderer) {
	return renderer->_viewport_size;
}

static void process_work(
	gfx::Renderer* const renderer
) {
	auto& w = renderer->_work_data;
	if (w.num_commands == 0) {
		return;
	}

	void const* data = w.buffer;
	gfx::CmdType type;
	for (; w.num_commands > 0; --w.num_commands) {
		type = *static_cast<gfx::CmdType const*>(data);
		data = pointer_add(data, sizeof(gfx::CmdType));
		data = pointer_add(data, renderer::execute_command(renderer, type, data));
	}
	w.num_commands = 0;
	w.buffer_size = 0;
}

static void worker_task_func(TaskID /*id*/, void* task_data) {
	auto* renderer = static_cast<gfx::Renderer*>(task_data);
	auto& w = renderer->_work_data;
	MutexLock l{renderer->_frame_mutex};
	gfx::display::bind_context(w.display);
	while (
		w.active ||
		w.num_commands > 0
	) {
		process_work(renderer);
		if (!w.active) {
			break;
		}
		condvar::wait(renderer->_frame_condvar, l);
	}
	gfx::display::swap_buffers(w.display);
	gfx::display::unbind_context();
	w.display = nullptr;
}

/// Begin frame.
///
/// This binds the display context to the worker thread. It must be
/// unbound on all other threads before calling this.
///
/// An assertion will fail if the frame has already begun.
TaskID renderer::begin_frame(
	gfx::Renderer* renderer,
	TaskManager& task_manager,
	gfx::Display* display
) {
	MutexLock l{renderer->_frame_mutex};
	auto& w = renderer->_work_data;
	TOGO_ASSERTE(!w.active);
	w.active = true;
	w.display = display;
	w.num_commands = 0;
	w.buffer_size = 0;
	return task_manager::add(
		task_manager, TaskWork{renderer, worker_task_func}
	);
}

/// End frame.
///
/// An assertion will fail if the frame has already ended.
void renderer::end_frame(gfx::Renderer* renderer) {
	MutexLock l{renderer->_frame_mutex};
	auto& w = renderer->_work_data;
	TOGO_ASSERTE(w.active);
	w.active = false;
	condvar::signal(renderer->_frame_condvar, l);
}

/// Push work.
///
/// command is copied to an internal buffer.
///
/// An assertion will fail if there is no space for another command.
void renderer::push_work(
	gfx::Renderer* const renderer,
	gfx::CmdType const type,
	unsigned const data_size,
	void const* const data
) {
	MutexLock l{renderer->_frame_mutex};
	auto& w = renderer->_work_data;
	TOGO_ASSERTE(w.active);

	unsigned const next_buffer_size = w.buffer_size + sizeof(gfx::CmdType) + data_size;
	TOGO_ASSERTE(sizeof(w.buffer) >= next_buffer_size);

	auto* put = static_cast<void*>(w.buffer + w.buffer_size);
	*static_cast<gfx::CmdType*>(put) = type;
	if (data_size > 0) {
		put = pointer_add(put, sizeof(gfx::CmdType));
		std::memcpy(put, data, data_size);
	}

	w.buffer_size = next_buffer_size;
	++w.num_commands;
	condvar::signal(renderer->_frame_condvar, l);
}

/// Execute command.
///
/// Returns the size of the command data.
unsigned renderer::execute_command(
	gfx::Renderer* const renderer,
	gfx::CmdType const type,
	void const* const data
) {
	switch (type) {
	case gfx::CmdType::Callback:
		// TODO
		return 0;

	case gfx::CmdType::ClearBackbuffer:
		gfx::renderer::clear_backbuffer(renderer);
		return 0;

	case gfx::CmdType::RenderBuffers: {
		auto* d = static_cast<gfx::CmdRenderBuffers const*>(data);
		gfx::renderer::render_buffers(
			renderer,
			d->shader_id,
			d->num_draw_param_blocks,
			d->draw_param_blocks,
			d->num_buffers,
			d->buffers
		);
		return sizeof(*d);
	}

	case gfx::CmdType::RenderWorld: {
		auto* d = static_cast<gfx::CmdRenderWorld const*>(data);
		gfx::renderer::render_world(
			renderer,
			d->world_id,
			d->camera_id,
			d->viewport_name_hash
		);
		return sizeof(*d);
	}
	}
	TOGO_ASSERTE(false);
}

namespace {
struct CmdKeyKeyFunc {
	inline u64 operator()(gfx::CmdKey const& key) const noexcept {
		return key.key;
	}
};
} // anonymous namespace

/// Render objects through camera and viewport.
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
		TOGO_ASSERTE(type != gfx::CmdType::RenderWorld);
		renderer::execute_command(renderer, type, data_untyped);
	}}
}

/// Render world through camera and viewport.
void renderer::render_world(
	gfx::Renderer* const renderer,
	WorldID const /*world_id*/,
	EntityID const /*camera_id*/,
	gfx::ViewportNameHash const viewport_name_hash
) {
	// TODO: Take mesh component manager and Camera object directly
	// (lower-level than app::render_world()). Rename, too.
	// TODO: Camera data from entity
	// TODO: Cull objects from world
	gfx::Camera const camera{};
	gfx::renderer::render_objects(
		renderer,
		0, nullptr,
		camera, viewport_name_hash
	);
}

} // namespace gfx
} // namespace game
} // namespace togo
