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
#include <togo/utility/utility.hpp>
#include <togo/gfx/types.hpp>
#include <togo/memory/memory.hpp>

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

/// Get generator definition by name.
gfx::GeneratorDef const* get_generator_def(
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

/** @} */ // end of doc-group gfx_renderer

} // namespace renderer
} // namespace gfx
} // namespace togo
