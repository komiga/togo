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

/// Create vertex buffer.
///
/// An assertion will fail if the vertex buffer could not be created.
gfx::VertexBufferID create_vertex_buffer(
	gfx::Renderer* renderer,
	void const* data,
	unsigned size,
	gfx::BufferDataBinding data_binding = gfx::BufferDataBinding::fixed
);

/// Destroy vertex buffer.
void destroy_vertex_buffer(
	gfx::Renderer* renderer,
	gfx::VertexBufferID id
);

/// Create index buffer.
///
/// An assertion will fail if the index buffer could not be created.
gfx::IndexBufferID create_index_buffer(
	gfx::Renderer* renderer,
	void const* data,
	unsigned size,
	gfx::BufferDataBinding data_binding = gfx::BufferDataBinding::fixed
);

/// Destroy index buffer.
void destroy_index_buffer(
	gfx::Renderer* renderer,
	gfx::IndexBufferID id
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

/** @} */ // end of doc-group gfx_renderer

} // namespace renderer
} // namespace gfx
} // namespace togo
