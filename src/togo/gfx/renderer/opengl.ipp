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

namespace {

inline static void bind_buffer(
	gfx::Renderer* const renderer,
	gfx::BufferID const id,
	GLenum const type
) {
	if (id.valid()) {
		auto const& buffer = resource_array::get(renderer->_buffers, id);
		TOGO_ASSERT(buffer.id == id, "invalid buffer ID");
		TOGO_GLCE_X(glBindBuffer(type, buffer.handle));
	} else {
		TOGO_GLCE_X(glBindBuffer(type, BUFFER_HANDLE_NULL));
	}
}

} // anonymous namespace

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

gfx::BufferID renderer::create_buffer(
	gfx::Renderer* const renderer,
	unsigned const size,
	void const* const data,
	gfx::BufferDataBinding const data_binding
) {
	TOGO_ASSERTE(size > 0);
	gfx::Buffer buffer{
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
	return resource_array::assign(renderer->_buffers, buffer).id;
}

void renderer::destroy_buffer(
	gfx::Renderer* const renderer,
	gfx::BufferID const id
) {
	auto& buffer = resource_array::get(renderer->_buffers, id);
	TOGO_ASSERT(buffer.id == id, "invalid buffer ID");
	TOGO_GLCE_X(glDeleteBuffers(1, &buffer.handle));
	resource_array::free(renderer->_buffers, buffer);
}

void renderer::map_buffer(
	gfx::Renderer* const renderer,
	gfx::BufferID const id,
	unsigned const offset,
	unsigned const size,
	void const* const data
) {
	TOGO_ASSERTE(
		size > 0 &&
		data
	);
	auto const& buffer = resource_array::get(renderer->_buffers, id);
	TOGO_ASSERT(buffer.id == id, "invalid buffer ID");
	TOGO_GLCE_X(glBindBuffer(GL_ARRAY_BUFFER, buffer.handle));
	TOGO_GLCE_X(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
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
	bind_buffer(renderer, index_binding.id, GL_ELEMENT_ARRAY_BUFFER);
	if (index_binding.id.valid()) {
		bb.flags
			= gfx::BufferBinding::F_INDEXED
			| (unsigned_cast(index_binding.type) << gfx::BufferBinding::F_SHIFT_INDEX_TYPE)
		;
	}

	{// Attach vertex bindings
	unsigned attrib_index = 0;
	for (auto const& vertex_binding : array_ref(num_bindings, bindings)) {
		TOGO_ASSERTE(vertex_binding.id.valid());
		bind_buffer(renderer, vertex_binding.id, GL_ARRAY_BUFFER);
		TOGO_DEBUG_ASSERTE(vertex_binding.format);
		auto const& format = *vertex_binding.format;
		for (unsigned i = 0; i < format._num_attribs; ++i) {
			auto const& attrib = format._attribs[i];
			// TODO: ... Does glVertexAttribIPointer() have to be used
			// for integral types?
			TOGO_GLCE_X(glVertexAttribPointer(
				attrib_index,
				attrib.num_components,
				gfx::g_gl_primitive_type[unsigned_cast(attrib.type)],
				attrib.normalize_fixed,
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

gfx::ShaderID renderer::create_shader(
	gfx::Renderer* const renderer,
	unsigned const num_stages,
	gfx::ShaderStage const* const stages
) {
	TOGO_DEBUG_ASSERTE(stages && num_stages > 0);
	TOGO_ASSERTE(num_stages <= unsigned_cast(gfx::ShaderStage::Type::NUM));

	enum { NUM_SOURCES = 24 };
	FixedArray<GLuint, 2> shader_handles;
	FixedArray<char const*, NUM_SOURCES> sources;
	FixedArray<GLint, NUM_SOURCES> sources_size;
	char info_log[1024];

	{// Shared internal prefix
	StringRef const internal_prefix{"#version 330\n"};
	fixed_array::push_back(sources, internal_prefix.data);
	fixed_array::push_back(sources_size, signed_cast(internal_prefix.size));
	}

	// Create GL shader handles
	for (unsigned i = 0; i < num_stages; ++i) {
		auto const& stage = stages[i];

		// Join sources
		fixed_array::resize(sources, 1);
		fixed_array::resize(sources_size, 1);
		for (StringRef const& stage_source : stage.sources) {
			fixed_array::push_back(sources, stage_source.data);
			fixed_array::push_back(sources_size, signed_cast(stage_source.size));
		}

		// Create GL shader
		GLenum const type = gfx::g_gl_shader_stage_type[unsigned_cast(stage.type)];
		if (i == 1) {
			TOGO_ASSERT(stage.type != stages[i - 1].type, "multiple stages of the same type");
		}

		GLuint shader_handle{PROGRAM_HANDLE_NULL};
		TOGO_GLCE_X(shader_handle = glCreateShader(type));
		TOGO_ASSERTE(shader_handle != PROGRAM_HANDLE_NULL);
		TOGO_GLCE_X(glShaderSource(
			shader_handle,
			fixed_array::size(sources),
			fixed_array::begin(sources),
			fixed_array::begin(sources_size)
		));
		TOGO_GLCE_X(glCompileShader(shader_handle));

		{// Check log
		GLsizei log_size{0};
		info_log[0] = '\0';
		glGetShaderInfoLog(shader_handle, array_extent(info_log), &log_size, info_log);
		if (log_size > 0) {
			TOGO_LOGF("GL shader log:\n%.*s\n", log_size, info_log);
		}}

		{// Check compilation status
		GLint status{GL_FALSE};
		TOGO_GLCE_X(glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &status));
		TOGO_ASSERTE(status != GL_FALSE);
		}

		fixed_array::push_back(shader_handles, shader_handle);
	}

	// Create GL program
	gfx::Shader shader{{}, PROGRAM_HANDLE_NULL};
	TOGO_GLCE_X(shader.handle = glCreateProgram());
	TOGO_GLCE_X(glBindFragDataLocation(shader.handle, 0, "RESULT0"));

	// Attach GL shaders and link
	for (auto const shader_handle : shader_handles) {
		TOGO_GLCE_X(glAttachShader(shader.handle, shader_handle));
	}
	TOGO_GLCE_X(glLinkProgram(shader.handle));

	{// Check log
	GLsizei log_size{0};
	info_log[0] = '\0';
	glGetProgramInfoLog(shader.handle, array_extent(info_log), &log_size, info_log);
	if (log_size > 0) {
		TOGO_LOGF("GL program log:\n%.*s\n", log_size, info_log);
	}}

	{// Check link status
	GLint status{GL_FALSE};
	TOGO_GLCE_X(glGetProgramiv(shader.handle, GL_LINK_STATUS, &status));
	TOGO_ASSERTE(status != GL_FALSE);
	}

	// Destroy GL shaders
	for (auto const shader_handle : shader_handles) {
		TOGO_GLCE_X(glDetachShader(shader.handle, shader_handle));
		TOGO_GLCE_X(glDeleteShader(shader_handle));
	}
	return resource_array::assign(renderer->_shaders, shader).id;
}

void renderer::destroy_shader(
	gfx::Renderer* const renderer,
	gfx::ShaderID const id
) {
	auto& shader = resource_array::get(renderer->_shaders, id);
	TOGO_ASSERT(shader.id == id, "invalid shader ID");
	TOGO_GLCE_X(glDeleteProgram(shader.handle));
	resource_array::free(renderer->_shaders, shader);
}

} // namespace gfx
} // namespace togo
