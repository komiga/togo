#line 2 "togo/gfx/renderer/opengl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/renderer/opengl.hpp>
#include <togo/gfx/renderer/private.hpp>

namespace togo {
namespace gfx {

char const* gl_get_error() {
	GLenum const error_code = glGetError();
	switch (error_code) {
	case GL_NO_ERROR:
		return nullptr;
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	case GL_STACK_OVERFLOW:
		return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:
		return "GL_STACK_UNDERFLOW";
	default:
		return "UNKNOWN";
	}
}

namespace renderer_opengl {

inline static void bind_vertex_buffer(
	gfx::Renderer* const renderer,
	gfx::VertexBufferID const id
) {
	if (id.valid()) {
		auto const& buffer = resource_array::get(renderer->_vertex_buffers, id);
		TOGO_ASSERT(buffer.id == id, "invalid vertex buffer ID");
		TOGO_GLCE_X(glBindBuffer(GL_ARRAY_BUFFER, buffer.handle));
	} else {
		TOGO_GLCE_X(glBindBuffer(GL_ARRAY_BUFFER, BUFFER_HANDLE_NULL));
	}
}

inline static void bind_index_buffer(
	gfx::Renderer* const renderer,
	gfx::IndexBufferID const id
) {
	if (id.valid()) {
		auto const& buffer = resource_array::get(renderer->_index_buffers, id);
		TOGO_ASSERT(buffer.id == id, "invalid index buffer ID");
		TOGO_GLCE_X(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.handle));
	} else {
		TOGO_GLCE_X(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUFFER_HANDLE_NULL));
	}
}

} // namespace renderer_opengl

gfx::Renderer* renderer::create(
	Allocator& allocator
) {
	gfx::Renderer* const renderer = TOGO_CONSTRUCT(
		allocator, gfx::Renderer,
		allocator,
		OpenGLRendererImpl{}
	);
	return renderer;
}

void renderer::destroy(gfx::Renderer* const renderer) {
	Allocator& allocator = *renderer->_allocator;
	renderer::teardown_base(renderer);
	TOGO_DESTROY(allocator, renderer);
}

gfx::RendererType renderer::type(gfx::Renderer const* /*renderer*/) {
	return gfx::RENDERER_TYPE_OPENGL;
}

inline static gfx::GLBufferFlags gl_buffer_flags(
	gfx::BufferDataBinding const data_binding
) {
	return (data_binding == gfx::BufferDataBinding::dynamic)
		? gfx::GLBufferFlags::dynamic
		: gfx::GLBufferFlags::none
	;
}

gfx::VertexBufferID renderer::create_vertex_buffer(
	gfx::Renderer* const renderer,
	void const* const data,
	unsigned const size,
	gfx::BufferDataBinding const data_binding
) {
	TOGO_ASSERTE(data);
	TOGO_ASSERTE(size > 0);
	gfx::VertexBuffer buffer{
		{}, BUFFER_HANDLE_NULL,
		gl_buffer_flags(data_binding)
	};
	TOGO_GLCE_X(glGenBuffers(1, &buffer.handle));
	TOGO_ASSERTE(buffer.handle != BUFFER_HANDLE_NULL);
	TOGO_GLCE_X(glBindBuffer(GL_ARRAY_BUFFER, buffer.handle));
	TOGO_GLCE_X(glBufferData(
		GL_ARRAY_BUFFER,
		size, data, g_gl_buffer_data_binding[unsigned_cast(data_binding)]
	));
	return resource_array::assign(renderer->_vertex_buffers, buffer).id;
}

void renderer::destroy_vertex_buffer(
	gfx::Renderer* const renderer,
	gfx::VertexBufferID const id
) {
	auto& buffer = resource_array::get(renderer->_vertex_buffers, id);
	TOGO_ASSERT(buffer.id == id, "invalid vertex buffer ID");
	TOGO_GLCE_X(glDeleteBuffers(1, &buffer.handle));
	resource_array::free(renderer->_vertex_buffers, buffer);
}

gfx::IndexBufferID renderer::create_index_buffer(
	gfx::Renderer* const renderer,
	void const* const data,
	unsigned const size,
	gfx::BufferDataBinding const data_binding
) {
	TOGO_ASSERTE(data);
	TOGO_ASSERTE(size > 0);
	gfx::IndexBuffer buffer{
		{}, BUFFER_HANDLE_NULL,
		gl_buffer_flags(data_binding)
	};
	TOGO_GLCE_X(glGenBuffers(1, &buffer.handle));
	TOGO_ASSERTE(buffer.handle != BUFFER_HANDLE_NULL);
	TOGO_GLCE_X(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.handle));
	TOGO_GLCE_X(glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		size, data, g_gl_buffer_data_binding[unsigned_cast(data_binding)]
	));
	return resource_array::assign(renderer->_index_buffers, buffer).id;
}

void renderer::destroy_index_buffer(
	gfx::Renderer* const renderer,
	gfx::IndexBufferID const id
) {
	auto& buffer = resource_array::get(renderer->_index_buffers, id);
	TOGO_ASSERT(buffer.id == id, "invalid index buffer ID");
	TOGO_GLCE_X(glDeleteBuffers(1, &buffer.handle));
	resource_array::free(renderer->_index_buffers, buffer);
}

gfx::BufferBindingID renderer::create_buffer_binding(
	gfx::Renderer* const renderer,
	unsigned const num_vertices,
	unsigned const base_vertex,
	gfx::IndexBinding const& index_binding,
	unsigned const num_bindings,
	gfx::VertexBinding const* const bindings
) {
	TOGO_ASSERTE(
		base_vertex < num_vertices &&
		num_vertices > 0 &&
		num_bindings > 0 &&
		bindings
	);
	gfx::BufferBinding bb{
		{}, VERTEX_ARRAY_HANDLE_NULL,
		base_vertex,
		num_vertices,
		gfx::BufferBinding::F_NONE
	};

	// Create vertex array
	TOGO_GLCE_X(glGenVertexArrays(1, &bb.va_handle));
	TOGO_ASSERTE(bb.va_handle != VERTEX_ARRAY_HANDLE_NULL);
	TOGO_GLCE_X(glBindVertexArray(bb.va_handle));
	gfx::renderer_opengl::bind_index_buffer(renderer, index_binding.id);
	if (index_binding.id.valid()) {
		bb.flags
			= gfx::BufferBinding::F_INDEXED
			| (unsigned_cast(index_binding.type) << gfx::BufferBinding::F_SHIFT_INDEX_TYPE)
		;
	}

	{// Attach vertex bindings
	unsigned attrib_index = 0;
	GLenum attrib_type;
	for (auto const& vertex_binding : array_ref(num_bindings, bindings)) {
		TOGO_ASSERTE(vertex_binding.id.valid());
		gfx::renderer_opengl::bind_vertex_buffer(renderer, vertex_binding.id);
		TOGO_DEBUG_ASSERTE(vertex_binding.format);
		auto const& format = *vertex_binding.format;
		for (unsigned i = 0; i < format._num_attribs; ++i) {
			auto const& attrib = format._attribs[i];
			attrib_type = gfx::g_gl_vertex_attrib_type[unsigned_cast(attrib.type)];
			// TODO: ... Does glVertexAttribIPointer() have to be used
			// for integral types?
			TOGO_GLCE_X(glVertexAttribPointer(
				attrib_index,
				attrib.num_components, attrib_type, attrib.normalize_fixed,
				format._stride,
				reinterpret_cast<void const*>(
					vertex_binding.offset + format._offsets[i]
				)
			));
			TOGO_GLCE_X(glEnableVertexAttribArray(attrib_index));
			// TODO: Proper attribute index stride for matrices (and arrays?)
			++attrib_index;
		}
	}}
	return resource_array::assign(renderer->_buffer_bindings, bb).id;
}

void renderer::destroy_buffer_binding(
	gfx::Renderer* const renderer,
	gfx::BufferBindingID const id
) {
	auto& bb = resource_array::get(renderer->_buffer_bindings, id);
	TOGO_ASSERT(bb.id == id, "invalid buffer binding ID");
	TOGO_GLCE_X(glDeleteVertexArrays(1, &bb.va_handle));
	resource_array::free(renderer->_buffer_bindings, bb);
}

} // namespace gfx
} // namespace togo
