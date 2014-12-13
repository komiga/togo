#line 2 "togo/gfx/renderer/opengl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
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

} // namespace gfx
} // namespace togo
