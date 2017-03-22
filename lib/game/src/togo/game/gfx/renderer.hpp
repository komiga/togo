#line 2 "togo/game/gfx/renderer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Renderer interface.
@ingroup lib_game_gfx
@ingroup lib_game_gfx_renderer

@defgroup lib_game_gfx_renderer Renderer
@ingroup lib_game_gfx
@details
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/math/vector/2_type.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/window/window/types.hpp>
#include <togo/game/entity/types.hpp>
#include <togo/game/world/types.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/command.hpp>
#include <togo/game/gfx/renderer.gen_interface>

#include <initializer_list>

namespace togo {
namespace game {
namespace gfx {
namespace renderer {

/**
	@addtogroup lib_game_gfx_renderer
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

/// Type.
gfx::RendererType type(gfx::Renderer const* renderer);

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
	gfx::ParamBlockBinding const& binding,
	void const* data
) {
	renderer::map_buffer(
		renderer, binding.id, binding.offset, binding.size, data
	);
}

/// Parameter block offset by index.
///
/// block_size should be the size of the largest block in the buffer.
unsigned param_block_offset(
	gfx::Renderer const* renderer,
	unsigned block_index,
	unsigned block_size
);

/// Size for a buffer of parameter blocks.
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
gfx::ParamBlockBinding make_param_block_binding(
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
	gfx::VertexBinding const* bindings,
	gfx::PolygonizationMethod polygonization_method = gfx::PolygonizationMethod::triangles
);

/// Create buffer binding by initializer list.
inline gfx::BufferBindingID create_buffer_binding(
	gfx::Renderer* renderer,
	unsigned num_vertices,
	unsigned base_vertex,
	gfx::IndexBinding const& index_binding,
	std::initializer_list<gfx::VertexBinding> const bindings,
	gfx::PolygonizationMethod polygonization_method = gfx::PolygonizationMethod::triangles
) {
	return gfx::renderer::create_buffer_binding(
		renderer,
		num_vertices, base_vertex, index_binding,
		bindings.size(), bindings.begin(),
		polygonization_method
	);
}

/// Destroy buffer binding.
void destroy_buffer_binding(
	gfx::Renderer* renderer,
	gfx::BufferBindingID id
);

/// Create render target.
///
/// An assertion will fail if the render target could not be created.
gfx::RenderTargetID create_render_target(
	gfx::Renderer* renderer,
	gfx::RenderTargetSpec const& spec
);

/// Destroy render target.
void destroy_render_target(
	gfx::Renderer* renderer,
	gfx::RenderTargetID id
);

/// Create framebuffer.
///
/// depth_stencil_target may be invalid.
///
/// An assertion will fail if the framebuffer could not be created.
gfx::FramebufferID create_framebuffer(
	gfx::Renderer* renderer,
	unsigned num_color_targets,
	gfx::RenderTargetID const* color_targets,
	gfx::RenderTargetID depth_stencil_target
);

/// Destroy framebuffer.
void destroy_framebuffer(
	gfx::Renderer* renderer,
	gfx::FramebufferID id
);

/// Active framebuffer.
///
/// If the default framebuffer is bound, this returns an invalid ID.
gfx::FramebufferID active_framebuffer(
	gfx::Renderer* renderer
);

/// Bind framebuffer.
///
/// If id is invalid, the default framebuffer is bound.
void bind_framebuffer(
	gfx::Renderer* renderer,
	gfx::FramebufferID id
);

/// Create shader.
///
/// An assertion will fail if the shader could not be created.
gfx::ShaderID create_shader(
	gfx::Renderer* renderer,
	gfx::ShaderSpec const& spec
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
	UVec2 size
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

/// Clear the backbuffer.
void clear_backbuffer(
	gfx::Renderer* renderer
);

/// Render a fullscreen pass.
void render_fullscreen_pass(
	gfx::Renderer* renderer,
	gfx::ShaderID shader_id,
	gfx::FramebufferID framebuffer_id,
	gfx::RenderTargetID output_id
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
/// Base effects:
///
/// 1. Destroys previous shared resources.
/// 2. Copies the config to the renderer.
/// 3. Creates shared resources.
/// 4. Generator definitions are reinitialized.
/// 5. Generator units are deserialized from the packed unit data.
///
/// An assertion will fail if the renderer could not be configured.
void configure(
	gfx::Renderer* renderer,
	gfx::PackedRenderConfig const& packed_config,
	Endian endian = Endian::little
);

/** @} */ // end of doc-group lib_game_gfx_renderer

} // namespace renderer
} // namespace gfx
} // namespace game
} // namespace togo
