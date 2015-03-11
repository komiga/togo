#line 2 "togo/gfx/renderer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Renderer interface.
@ingroup gfx
@ingroup gfx_renderer
*/

#pragma once

#include <togo/config.hpp>
#include <togo/threading/types.hpp>
#include <togo/entity/types.hpp>
#include <togo/world/types.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/command.hpp>

#include <initializer_list>

namespace togo {
namespace gfx {
namespace renderer {

/**
	@addtogroup gfx_renderer
	@{
*/

/// Create renderer.
///
/// An assertion will fail if the renderer could not be created.
gfx::Renderer* create(
	Allocator& allocator = memory::default_allocator()
);

/// Destroy renderer.
void destroy(gfx::Renderer* renderer);

/// Renderer type.
gfx::RendererType type(gfx::Renderer const* renderer);

/// Register generator definition.
///
/// An assertion will fail if the generator definition is malformed
/// or is already registered.
void register_generator_def(
	gfx::Renderer* renderer,
	gfx::GeneratorDef const& def
);

/// Find generator definition by name.
gfx::GeneratorDef const* find_generator_def(
	gfx::Renderer const* renderer,
	gfx::GeneratorNameHash name_hash
);

/// Create buffer.
///
/// If data is nullptr, map_buffer() must be used to fill the buffer.
/// An assertion will fail if the buffer could not be created.
gfx::BufferID create_buffer(
	gfx::Renderer* renderer,
	unsigned size,
	void const* data = nullptr,
	gfx::BufferDataBinding data_binding = gfx::BufferDataBinding::fixed
);

/// Destroy buffer.
void destroy_buffer(
	gfx::Renderer* renderer,
	gfx::BufferID id
);

/// Map data to buffer.
void map_buffer(
	gfx::Renderer* renderer,
	gfx::BufferID id,
	unsigned offset,
	unsigned size,
	void const* data
);

/// Map data to buffer by parameter block binding.
inline void map_buffer(
	gfx::Renderer* renderer,
	ParamBlockBinding const& binding,
	void const* data
) {
	renderer::map_buffer(
		renderer, binding.id, binding.offset, binding.size, data
	);
}

/// Get parameter block offset by index.
///
/// block_size should be the size of the largest block in the buffer.
unsigned param_block_offset(
	gfx::Renderer const* renderer,
	unsigned block_index,
	unsigned block_size
);

/// Get size for a buffer of parameter blocks.
///
/// This should be used for buffers that contain parameter blocks,
/// which require aligned offsets.
unsigned param_block_buffer_size(
	gfx::Renderer const* renderer,
	unsigned num_blocks,
	unsigned block_size
);

/// Make a parameter block binding.
///
/// Offset must be aligned with param_block_offset().
ParamBlockBinding make_param_block_binding(
	gfx::Renderer const* renderer,
	gfx::BufferID id,
	unsigned offset,
	unsigned size
);

/// Create buffer binding.
///
/// An assertion will fail if the buffer binding could not be created.
gfx::BufferBindingID create_buffer_binding(
	gfx::Renderer* renderer,
	unsigned num_vertices,
	unsigned base_vertex,
	gfx::IndexBinding const& index_binding,
	unsigned num_bindings,
	gfx::VertexBinding const* bindings
);

/// Create buffer binding by initializer list.
inline gfx::BufferBindingID create_buffer_binding(
	gfx::Renderer* renderer,
	unsigned num_vertices,
	unsigned base_vertex,
	gfx::IndexBinding const& index_binding,
	std::initializer_list<gfx::VertexBinding> const bindings
) {
	return gfx::renderer::create_buffer_binding(
		renderer,
		num_vertices, base_vertex, index_binding,
		bindings.size(), bindings.begin()
	);
}

/// Destroy buffer binding.
void destroy_buffer_binding(
	gfx::Renderer* renderer,
	gfx::BufferBindingID id
);

/// Create shader.
///
/// An assertion will fail if the shader could not be created.
gfx::ShaderID create_shader(
	gfx::Renderer* renderer,
	ShaderSpec const& spec
);

/// Destroy shader.
void destroy_shader(
	gfx::Renderer* renderer,
	gfx::ShaderID id
);

/// Set a fixed parameter block by index.
///
/// An assertion will fail if name_hash is PB_NAME_NULL.
/// index must be in [0, 15].
void set_fixed_param_block(
	gfx::Renderer* renderer,
	unsigned index,
	gfx::ParamBlockNameHash name_hash,
	gfx::ParamBlockBinding const& binding
);

/// Unset a fixed parameter block by index.
void unset_fixed_param_block(
	gfx::Renderer* renderer,
	unsigned index
);

/// Set viewport size.
void set_viewport_size(
	gfx::Renderer* renderer,
	unsigned width,
	unsigned height
);

/// Begin frame.
///
/// This binds the display context to the worker thread. It must be
/// unbound on all other threads before calling this.
///
/// An assertion will fail if the frame has already begun.
TaskID begin_frame(
	gfx::Renderer* renderer,
	TaskManager& task_manager,
	gfx::Display* display
);

/// End frame.
///
/// An assertion will fail if the frame has already ended.
void end_frame(gfx::Renderer* renderer);

/// Push work.
///
/// command is copied to an internal buffer.
///
/// An assertion will fail if there is no space for another command.
void push_work(
	gfx::Renderer* renderer,
	gfx::CmdType type,
	unsigned data_size,
	void const* data
);

/// Push work (generic helper).
template<class T>
inline void push_work(
	gfx::Renderer* renderer,
	T const& data
) {
	renderer::push_work(
		renderer,
		CmdTypeProperties<T>::type,
		sizeof_empty<T>(), &data
	);
}

/// Execute command.
///
/// Returns the size of the command data.
unsigned execute_command(
	gfx::Renderer* renderer,
	gfx::CmdType type,
	void const* data
);

/// Clear the backbuffer.
void clear_backbuffer(
	gfx::Renderer* renderer
);

/// Render buffers.
void render_buffers(
	gfx::Renderer* renderer,
	gfx::ShaderID shader_id,
	unsigned num_draw_param_blocks,
	gfx::ParamBlockBinding const* draw_param_blocks,
	unsigned num_buffers,
	gfx::BufferBindingID const* buffers
);

/// Configure the renderer.
///
/// An assertion will fail if the renderer could not be configured.
void configure(
	gfx::Renderer* renderer,
	gfx::RenderConfig const& config
);

/// Render objects through camera and viewport.
void render_objects(
	gfx::Renderer* renderer,
	unsigned num_objects,
	gfx::RenderObject const* objects,
	gfx::Camera const& camera,
	gfx::ViewportNameHash viewport_name_hash
);

/// Render world through camera and viewport.
void render_world(
	gfx::Renderer* renderer,
	WorldID world_id,
	EntityID camera_id,
	gfx::ViewportNameHash viewport_name_hash
);

/** @} */ // end of doc-group gfx_renderer

} // namespace renderer
} // namespace gfx
} // namespace togo
