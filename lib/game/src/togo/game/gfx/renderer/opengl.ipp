#line 2 "togo/game/gfx/renderer/opengl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/vector/2.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/window/opengl.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/gfx/renderer/private.hpp>
#include <togo/game/gfx/renderer/opengl.hpp>

namespace togo {
namespace game {
namespace gfx {

namespace {

enum : unsigned {
	// First non-fixed param block buffer index.
	BASE_DRAW_PB_INDEX = TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND,
};

static void query_parameters(
	gfx::Renderer* const renderer
) {
	GLint gl_integer;
	#define TOGO_GET_INTEGER(pname_, min_, value_)								\
		glGetIntegerv(pname_, &gl_integer);										\
		renderer->_impl.value_ = max(min_, static_cast<unsigned>(gl_integer))

	TOGO_GET_INTEGER(
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
		1u, p_uniform_buffer_offset_alignment
	);
	TOGO_GET_INTEGER(
		GL_MAX_UNIFORM_BLOCK_SIZE,
		16384u, p_max_uniform_block_size
	);

	TOGO_LOGF(
		"GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT = %u\n"
		"GL_MAX_UNIFORM_BLOCK_SIZE = %u\n"
		, renderer->_impl.p_uniform_buffer_offset_alignment
		, renderer->_impl.p_max_uniform_block_size
	);

	#undef TOGO_GET_INTEGER
}

inline static unsigned align_param_block_offset(
	unsigned offset,
	unsigned const align
) {
	if (align > 1) {
		unsigned const m = offset % align;
		if (m > 0) {
			offset += align - m;
		}
	}
	return offset;
}

// null ID unbinds the target
inline static void bind_buffer(
	gfx::Renderer* const renderer,
	gfx::BufferID const id,
	GLenum const target
) {
	if (id.valid()) {
		auto const& buffer = resource_array::get(renderer->_buffers, id);
		TOGO_ASSERT(buffer.id == id, "invalid buffer ID");
		glBindBuffer(target, buffer.handle);
	} else {
		glBindBuffer(target, BUFFER_HANDLE_NULL);
	}
}

inline static void bind_param_block(
	gfx::Renderer* const renderer,
	gfx::BufferID const id,
	unsigned const index,
	unsigned const offset,
	unsigned const size
) {
	TOGO_ASSERTE(id.valid());
	auto const& buffer = resource_array::get(renderer->_buffers, id);
	TOGO_ASSERT(buffer.id == id, "invalid buffer ID");
	glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer.handle, offset, size);
}

inline static void unbind_param_block(
	gfx::Renderer* const /*renderer*/,
	unsigned const index
) {
	glBindBufferBase(GL_UNIFORM_BUFFER, index, BUFFER_HANDLE_NULL);
}

inline static void setup_param_block_bindings(
	gfx::Shader const& shader,
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> const& param_blocks,
	unsigned const base_buffer_index
) {
	GLuint block_index{GL_INVALID_INDEX};
	for (auto const& pb_def : param_blocks) {
		block_index = glGetUniformBlockIndex(shader.handle, pb_def.name.data);
		TOGO_ASSERTE(block_index != GL_INVALID_INDEX);
		glUniformBlockBinding(
			shader.handle, block_index, base_buffer_index + pb_def.index
		);
	}
}

// Validate against current state
static void validate_shader(
	gfx::Renderer* const renderer,
	gfx::Shader const& shader,
	unsigned const num_draw_param_blocks
) {
	// Parameter block sanity checks
	TOGO_ASSERTF(
		num_draw_param_blocks == shader.num_draw_param_blocks(),
		"number of supplied parameter blocks does not match the shader (%u != %u)",
		num_draw_param_blocks, shader.num_draw_param_blocks()
	);
	// Ensure fixed param blocks are supplied
	for (unsigned index = 0; index < BASE_DRAW_PB_INDEX; ++index) {
		if ((shader.param_block_bits >> index) & 1) {
			TOGO_ASSERTF(
				renderer->_fixed_param_blocks[index] != PB_NAME_NULL,
				"fixed parameter block required by shader is not set: index = %u",
				index
			);
		}
	}

	{// GL validation
	GLsizei log_size{0};
	char info_log[512];
	info_log[0] = '\0';
	GLint status{GL_FALSE};
	glValidateProgram(shader.handle);
	glGetProgramiv(shader.handle, GL_VALIDATE_STATUS, &status);
	glGetProgramInfoLog(shader.handle, array_extent(info_log), &log_size, info_log);

	// Status log is fluff when it succeeds
	if (status == GL_FALSE && log_size > 0) {
		TOGO_LOGF("shader validation log:\n%.*s\n", log_size, info_log);
	}
	TOGO_ASSERTE(status != GL_FALSE);
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
	renderer::init_base(renderer);
	query_parameters(renderer);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	renderer->_viewport_size.x = unsigned_cast(max(0, vp[2]));
	renderer->_viewport_size.y = unsigned_cast(max(0, vp[3]));

	// Check extensions
	for (auto const& extension : gl_extensions) {
		TOGO_ASSERTF(
			*extension.value,
			"required OpenGL extension '%.*s' is not available",
			extension.name.size, extension.name.data
		);
	}

	{// Create empty buffer binding
	gfx::BufferBinding bb{
		{}, VERTEX_ARRAY_HANDLE_NULL,
		0, 0, gfx::BufferBinding::F_NONE
	};

	glGenVertexArrays(1, &bb.va_handle);
	TOGO_ASSERTE(bb.va_handle != VERTEX_ARRAY_HANDLE_NULL);
	bind_buffer(renderer, {ID_VALUE_NULL}, GL_ARRAY_BUFFER);
	bind_buffer(renderer, {ID_VALUE_NULL}, GL_ELEMENT_ARRAY_BUFFER);
	glBindVertexArray(bb.va_handle);
	glBindVertexArray(VERTEX_ARRAY_HANDLE_NULL);

	renderer->_impl.empty_buffer_binding = &resource_array::assign(
		renderer->_buffer_bindings, bb
	);
	}

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

inline static u32 gl_buffer_flags(
	gfx::BufferDataBinding const data_binding
) {
	return (data_binding == gfx::BufferDataBinding::dynamic)
		? gfx::Buffer::F_DYNAMIC
		: gfx::Buffer::F_NONE
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
	glGenBuffers(1, &buffer.handle);
	TOGO_ASSERTE(buffer.handle != BUFFER_HANDLE_NULL);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.handle);
	glBufferData(
		GL_ARRAY_BUFFER,
		size, data, gfx::g_gl_buffer_data_binding[unsigned_cast(data_binding)]
	);
	return resource_array::assign(renderer->_buffers, buffer).id;
}

void renderer::destroy_buffer(
	gfx::Renderer* const renderer,
	gfx::BufferID const id
) {
	auto& buffer = resource_array::get(renderer->_buffers, id);
	TOGO_ASSERT(buffer.id == id, "invalid buffer ID");
	glDeleteBuffers(1, &buffer.handle);
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
	glBindBuffer(GL_ARRAY_BUFFER, buffer.handle);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

// NB: These two are equivalent!
unsigned renderer::param_block_offset(
	gfx::Renderer const* renderer,
	unsigned block_index,
	unsigned block_size
) {
	block_size = align_param_block_offset(
		block_size, renderer->_impl.p_uniform_buffer_offset_alignment
	);
	return block_index * block_size;
}

unsigned renderer::param_block_buffer_size(
	gfx::Renderer const* renderer,
	unsigned num_blocks,
	unsigned block_size
) {
	block_size = align_param_block_offset(
		block_size, renderer->_impl.p_uniform_buffer_offset_alignment
	);
	return num_blocks * block_size;
}

gfx::ParamBlockBinding renderer::make_param_block_binding(
	gfx::Renderer const* const renderer,
	gfx::BufferID const id,
	unsigned const offset,
	unsigned const size
) {
	TOGO_DEBUG_ASSERTE(size <= renderer->_impl.p_max_uniform_block_size);
	return {id, offset, size};
}

gfx::BufferBindingID renderer::create_buffer_binding(
	gfx::Renderer* const renderer,
	unsigned const num_vertices,
	unsigned const base_vertex,
	gfx::IndexBinding const& index_binding,
	unsigned const num_bindings,
	gfx::VertexBinding const* const bindings,
	gfx::PolygonizationMethod polygonization_method
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
	bb.flags
		|= (unsigned_cast(polygonization_method) & gfx::BufferBinding::M_POLYGONIZATION_METHOD)
		<< gfx::BufferBinding::S_POLYGONIZATION_METHOD
	;

	// Create vertex array
	glGenVertexArrays(1, &bb.va_handle);
	TOGO_ASSERTE(bb.va_handle != VERTEX_ARRAY_HANDLE_NULL);
	glBindVertexArray(bb.va_handle);
	bind_buffer(renderer, index_binding.id, GL_ELEMENT_ARRAY_BUFFER);
	if (index_binding.id.valid()) {
		bb.flags
			|= gfx::BufferBinding::F_INDEXED
			| (unsigned_cast(index_binding.type) << gfx::BufferBinding::F_SHIFT_INDEX_TYPE)
		;
	}

	{// Attach vertex bindings
	unsigned attrib_index = 0;
	for (auto const& vertex_binding : array_ref(bindings, num_bindings)) {
		TOGO_ASSERTE(vertex_binding.id.valid());
		bind_buffer(renderer, vertex_binding.id, GL_ARRAY_BUFFER);
		TOGO_DEBUG_ASSERTE(vertex_binding.format);
		auto const& format = *vertex_binding.format;
		for (unsigned i = 0; i < format._num_attribs; ++i) {
			auto const& attrib = format._attribs[i];
			if (gfx::is_primitive_integral(attrib.type)) {
				glVertexAttribIPointer(
					attrib_index,
					attrib.num_components,
					gfx::g_gl_primitive_type[unsigned_cast(attrib.type)],
					format._stride,
					reinterpret_cast<void const*>(
						vertex_binding.offset + format._offsets[i]
					)
				);
			} else {
				glVertexAttribPointer(
					attrib_index,
					attrib.num_components,
					gfx::g_gl_primitive_type[unsigned_cast(attrib.type)],
					attrib.normalize_fixed,
					format._stride,
					reinterpret_cast<void const*>(
						vertex_binding.offset + format._offsets[i]
					)
				);
			}
			glEnableVertexAttribArray(attrib_index);
			// TODO: Proper attribute index stride for matrices (and arrays?)
			++attrib_index;
			TOGO_DEBUG_ASSERTE(attrib_index <= TOGO_GFX_VERTEXFORMAT_NUM_ATTRIBS);
		}
	}
	TOGO_DEBUG_ASSERTE(0 < attrib_index);
	}

	bind_buffer(renderer, {ID_VALUE_NULL}, GL_ARRAY_BUFFER);
	bind_buffer(renderer, {ID_VALUE_NULL}, GL_ELEMENT_ARRAY_BUFFER);
	glBindVertexArray(VERTEX_ARRAY_HANDLE_NULL);

	return resource_array::assign(renderer->_buffer_bindings, bb).id;
}

void renderer::destroy_buffer_binding(
	gfx::Renderer* const renderer,
	gfx::BufferBindingID const id
) {
	auto& bb = resource_array::get(renderer->_buffer_bindings, id);
	TOGO_ASSERT(bb.id == id, "invalid buffer binding ID");
	glDeleteVertexArrays(1, &bb.va_handle);
	resource_array::free(renderer->_buffer_bindings, bb);
}

gfx::RenderTargetID renderer::create_render_target(
	gfx::Renderer* const renderer,
	gfx::RenderTargetSpec const& spec_in
) {
	TOGO_ASSERTE(spec_in.format < gfx::RenderTargetFormat::NUM);
	TOGO_ASSERTE(
		(spec_in.dim_x > 0.0f && spec_in.dim_y > 0.0f) ||
		spec_in.properties & gfx::RenderTargetSpec::F_SCALE
	);

	gfx::RenderTarget render_target{
		{}, TEXTURE_HANDLE_NULL,
		spec_in
	};
	auto& spec = render_target.spec;
	auto& format_info = gfx::g_gl_render_target_format[unsigned_cast(spec.format)];
	spec.properties &= ~gfx::RenderTarget::MASK_EXTENDED;
	spec.properties |= format_info.properties;
	auto const type
		= (
			spec.properties & gfx::RenderTargetSpec::F_DOUBLE_BUFFERED ||
			render_target.data_type() == gfx::RenderTarget::DT_COLOR
		)
		? gfx::RenderTarget::TYPE_TEXTURE
		: gfx::RenderTarget::TYPE_BUFFER
	;
	spec.properties |= type;

	Vec2 size{spec.dim_x, spec.dim_y};
	if (spec.properties & gfx::RenderTargetSpec::F_SCALE) {
		size *= renderer->_viewport_size;
	}

	switch (type) {
	case gfx::RenderTarget::TYPE_TEXTURE: {
		glGenTextures(1, &render_target.handle);
		TOGO_ASSERTE(render_target.handle != TEXTURE_HANDLE_NULL);

		GLenum const target
			= spec.properties & gfx::RenderTargetSpec::F_DOUBLE_BUFFERED
			? GL_TEXTURE_2D_ARRAY
			: GL_TEXTURE_2D
		;
		glBindTexture(target, render_target.handle);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (target == GL_TEXTURE_2D) {
			glTexImage2D(
				target, 0,
				format_info.data_format,
				size.x, size.y, 0,
				format_info.data_layout,
				format_info.data_type,
				nullptr
			);
		} else {
			glTexImage3D(
				target, 0,
				format_info.data_format,
				size.x, size.y, 2, 0,
				format_info.data_layout,
				format_info.data_type,
				nullptr
			);
		}
		glBindTexture(target, TEXTURE_HANDLE_NULL);
	}	break;

	case gfx::RenderTarget::TYPE_BUFFER: {
		glGenRenderbuffers(1, &render_target.handle);
		TOGO_ASSERTE(render_target.handle != RENDER_BUFFER_HANDLE_NULL);
		glBindRenderbuffer(GL_RENDERBUFFER, render_target.handle);
		glRenderbufferStorage(
			GL_RENDERBUFFER,
			format_info.data_format,
			size.x, size.y
		);
		glBindRenderbuffer(GL_RENDERBUFFER, RENDER_BUFFER_HANDLE_NULL);
	}	break;

	default:
		TOGO_DEBUG_ASSERTE(false);
	}
	return resource_array::assign(renderer->_render_targets, render_target).id;
}

void renderer::destroy_render_target(
	gfx::Renderer* const renderer,
	gfx::RenderTargetID const id
) {
	auto& render_target = resource_array::get(renderer->_render_targets, id);
	TOGO_ASSERT(render_target.id == id, "invalid render target ID");

	auto const& spec = render_target.spec;
	switch (spec.properties & gfx::RenderTarget::MASK_TYPE) {
	case gfx::RenderTarget::TYPE_TEXTURE:
		glDeleteTextures(1, &render_target.handle);
		break;

	case gfx::RenderTarget::TYPE_BUFFER:
		glDeleteRenderbuffers(1, &render_target.handle);
		break;

	default:
		TOGO_DEBUG_ASSERTE(false);
		break;
	}
	resource_array::free(renderer->_render_targets, render_target);
}

static unsigned attach_render_target(
	gfx::RenderTarget const& rt,
	unsigned const index
) {
	GLenum const attachment = gfx::gl_rt_attachment_for_data_type[rt.data_type()] + index;
	switch (rt.spec.properties & gfx::RenderTarget::MASK_TYPE) {
	case gfx::RenderTarget::TYPE_TEXTURE:
		if (rt.spec.properties & gfx::RenderTargetSpec::F_DOUBLE_BUFFERED) {
			glFramebufferTextureLayer(
				GL_FRAMEBUFFER, attachment, rt.handle, 0, 0
			);
			glFramebufferTextureLayer(
				GL_FRAMEBUFFER, attachment + 1, rt.handle, 0, 1
			);
			return 2;
		} else {
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, rt.handle, 0
			);
			return 1;
		}
		break;

	case gfx::RenderTarget::TYPE_BUFFER:
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rt.handle
		);
		return 1;
	}
	TOGO_ASSERTE(false);
	return 0;
}

gfx::FramebufferID renderer::create_framebuffer(
	gfx::Renderer* renderer,
	unsigned num_color_targets,
	gfx::RenderTargetID const* color_targets,
	gfx::RenderTargetID ds_target_id
) {
	TOGO_ASSERT(
		num_color_targets > 0 || ds_target_id.valid(),
		"framebuffer must have at least one color target or a depth-stencil target"
	);

	gfx::Framebuffer framebuffer{
		{}, FRAMEBUFFER_HANDLE_NULL,
		{}, {}
	};
	glGenFramebuffers(1, &framebuffer.handle);
	TOGO_ASSERTE(framebuffer.handle != FRAMEBUFFER_HANDLE_NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.handle);

	{// Bind color targets
	unsigned binding_index = 0;
	for (auto id : array_cref(color_targets, num_color_targets)) {
		TOGO_ASSERTE(id.valid());
		auto& render_target = resource_array::get(renderer->_render_targets, id);
		TOGO_ASSERT(render_target.id == id, "invalid render target ID");
		TOGO_ASSERT(
			render_target.data_type() == gfx::RenderTarget::DT_COLOR,
			"specified color target does not store color data"
		);
		fixed_array::push_back(framebuffer.color_targets, id);
		binding_index += attach_render_target(render_target, binding_index);
	}}

	// Bind depth-stencil target
	if (ds_target_id.valid()) {
		auto& ds_target = resource_array::get(renderer->_render_targets, ds_target_id);
		TOGO_ASSERT(ds_target.id == ds_target_id, "invalid render target ID");
		TOGO_ASSERT(
			ds_target.data_type() != gfx::RenderTarget::DT_COLOR,
			"specified depth-stencil target does not store depth and/or stencil data"
		);
		framebuffer.ds_target = ds_target_id;
		attach_render_target(ds_target, 0);
	}

	{// Check completeness
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	TOGO_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "framebuffer is incomplete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FRAMEBUFFER_HANDLE_NULL);
	return resource_array::assign(renderer->_framebuffers, framebuffer).id;
}

void renderer::destroy_framebuffer(
	gfx::Renderer* const renderer,
	gfx::FramebufferID const id
) {
	auto& framebuffer = resource_array::get(renderer->_framebuffers, id);
	TOGO_ASSERT(framebuffer.id == id, "invalid framebuffer ID");
	if (id == renderer->_active_framebuffer_id) {
		renderer::bind_framebuffer(renderer, {ID_VALUE_NULL});
	}
	glDeleteFramebuffers(1, &framebuffer.handle);
	resource_array::free(renderer->_framebuffers, framebuffer);
}

void renderer::bind_framebuffer(
	gfx::Renderer* const renderer,
	gfx::FramebufferID const id
) {
	if (id == renderer->_active_framebuffer_id) {
		return;
	} else if (id.valid()) {
		auto& framebuffer = resource_array::get(renderer->_framebuffers, id);
		TOGO_ASSERT(framebuffer.id == id, "invalid framebuffer ID");
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.handle);
	} else {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FRAMEBUFFER_HANDLE_NULL);
	}
	renderer->_active_framebuffer_id = id;
}

gfx::ShaderID renderer::create_shader(
	gfx::Renderer* const renderer,
	gfx::ShaderSpec const& spec
) {
	TOGO_DEBUG_ASSERTE(fixed_array::any(spec.stages));

	enum { NUM_SOURCES = 24 };
	FixedArray<GLuint, unsigned_cast(gfx::ShaderStage::Type::NUM)> shader_handles;
	FixedArray<char const*, NUM_SOURCES> sources;
	FixedArray<GLint, NUM_SOURCES> sources_size;
	char info_log[1024];

	{// Shared internal prefix
	StringRef const internal_prefix{"#version 330 core\n"};
	fixed_array::push_back(sources, internal_prefix.data);
	fixed_array::push_back(sources_size, signed_cast(internal_prefix.size));
	}

	// Create GL shader handles
	for (unsigned index = 0; index < fixed_array::size(spec.stages); ++index) {
		auto const& stage = spec.stages[index];

		// Join sources
		fixed_array::resize(sources, 1);
		fixed_array::resize(sources_size, 1);
		unsigned stage_index = 1;
		for (StringRef const& stage_source : stage.sources) {
			fixed_array::push_back(sources, stage_source.data);
			fixed_array::push_back(sources_size, signed_cast(stage_source.size));
			TOGO_LOG_DEBUGF(
				"GL shader source stage %u: ```%.*s```\n",
				stage_index,
				stage_source.size, stage_source.data
			);
			++stage_index;
		}

		// Create GL shader
		GLenum const type = gfx::g_gl_shader_stage_type[unsigned_cast(stage.type)];
		if (index == 1) {
			TOGO_ASSERT(
				stage.type != spec.stages[index - 1].type,
				"multiple stages of the same type"
			);
		}

		GLuint shader_handle{PROGRAM_HANDLE_NULL};
		shader_handle = glCreateShader(type);
		TOGO_ASSERTE(shader_handle != PROGRAM_HANDLE_NULL);
		glShaderSource(
			shader_handle,
			fixed_array::size(sources),
			fixed_array::begin(sources),
			fixed_array::begin(sources_size)
		);
		glCompileShader(shader_handle);

		{// Check log
		GLsizei log_size{0};
		info_log[0] = '\0';
		glGetShaderInfoLog(shader_handle, array_extent(info_log), &log_size, info_log);
		if (log_size > 0) {
			TOGO_LOGF("GL shader log:\n%.*s\n", log_size, info_log);
		}}

		{// Check compilation status
		GLint status{GL_FALSE};
		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &status);
		TOGO_ASSERTE(status != GL_FALSE);
		}

		fixed_array::push_back(shader_handles, shader_handle);
	}

	// Create GL program
	gfx::Shader shader{{}, PROGRAM_HANDLE_NULL, 0, 0};
	shader.handle = glCreateProgram();
	glBindFragDataLocation(shader.handle, 0, "RESULT0");

	// Attach GL shaders and link
	for (auto const shader_handle : shader_handles) {
		glAttachShader(shader.handle, shader_handle);
	}
	glLinkProgram(shader.handle);

	{// Check log
	GLsizei log_size{0};
	info_log[0] = '\0';
	glGetProgramInfoLog(shader.handle, array_extent(info_log), &log_size, info_log);
	if (log_size > 0) {
		TOGO_LOGF("GL program log:\n%.*s\n", log_size, info_log);
	}}

	{// Check link status
	GLint status{GL_FALSE};
	glGetProgramiv(shader.handle, GL_LINK_STATUS, &status);
	TOGO_ASSERTE(status != GL_FALSE);
	}

	// Destroy GL shaders
	for (auto const shader_handle : shader_handles) {
		glDetachShader(shader.handle, shader_handle);
		glDeleteShader(shader_handle);
	}

	// Setup param block bindings
	// TODO: Only do this for fixed param blocks
	// if context < 4.2 && !GL_ARB_shading_language_420pack.
	// shader-config is responsible for attaching them via
	// PARAM_BLOCK_*() based on this criteria.
	// Once the shader resource handler/compiler can rewrite
	// param block definitions, this can be extended to draw
	// param blocks as they will also be attached in the shader.
	setup_param_block_bindings(shader, spec.fixed_param_blocks, 0);
	setup_param_block_bindings(shader, spec.draw_param_blocks, BASE_DRAW_PB_INDEX);

	shader.properties
		|= (fixed_array::size(spec.fixed_param_blocks) << gfx::Shader::SHIFT_NUM_PB_FIXED)
		|  (fixed_array::size(spec.draw_param_blocks) << gfx::Shader::SHIFT_NUM_PB_DRAW)
	;
	for (auto const& pb_def : spec.fixed_param_blocks) {
		shader.param_block_bits |= 1 << pb_def.index;
	}

	return resource_array::assign(renderer->_shaders, shader).id;
}

void renderer::destroy_shader(
	gfx::Renderer* const renderer,
	gfx::ShaderID const id
) {
	auto& shader = resource_array::get(renderer->_shaders, id);
	TOGO_ASSERT(shader.id == id, "invalid shader ID");
	glDeleteProgram(shader.handle);
	resource_array::free(renderer->_shaders, shader);
}

void renderer::set_fixed_param_block(
	gfx::Renderer* const renderer,
	unsigned const index,
	gfx::ParamBlockNameHash const name_hash,
	gfx::ParamBlockBinding const& binding
) {
	TOGO_ASSERTE(
		index < BASE_DRAW_PB_INDEX &&
		name_hash != gfx::PB_NAME_NULL
	);
	bind_param_block(renderer, binding.id, index, binding.offset, binding.size);
	renderer->_fixed_param_blocks[index] = name_hash;
}

void renderer::unset_fixed_param_block(
	gfx::Renderer* const renderer,
	unsigned const index
) {
	TOGO_ASSERTE(index < BASE_DRAW_PB_INDEX);
	unbind_param_block(renderer, index);
	renderer->_fixed_param_blocks[index] = PB_NAME_NULL;
}

void renderer::set_viewport_size(
	gfx::Renderer* const renderer,
	UVec2 const size
) {
	glViewport(0, 0, size.x, size.y);
	renderer->_viewport_size = size;
}

void renderer::clear_backbuffer(
	gfx::Renderer* const /*renderer*/
) {
	Vec4 const clear_color{0.0f, 0.0f, 0.0f, 1.0f};
	glClearBufferfv(GL_COLOR, 0, &clear_color.x);
	glClearBufferfi(GL_DEPTH_STENCIL, 0, 0.0f, 0);
}

void renderer::render_fullscreen_pass(
	gfx::Renderer* const renderer,
	gfx::ShaderID const shader_id,
	gfx::FramebufferID const framebuffer_id,
	gfx::RenderTargetID const /*output_id*/
) {
	auto prev_framebuffer_id = gfx::renderer::active_framebuffer(renderer);
	gfx::renderer::bind_framebuffer(renderer, framebuffer_id);
	// TODO: uniform interface
	// gfx::renderer::set_uniform(renderer, shader, "u_fs_input", output_id);
	auto bb = renderer->_impl.empty_buffer_binding;
	bb->base_vertex = 0;
	bb->num_vertices = 3;
	gfx::renderer::render_buffers(renderer, shader_id, 0, nullptr, 1, &bb->id);
	gfx::renderer::bind_framebuffer(renderer, prev_framebuffer_id);
}

void renderer::render_buffers(
	gfx::Renderer* const renderer,
	gfx::ShaderID const shader_id,
	unsigned const num_draw_param_blocks,
	gfx::ParamBlockBinding const* const draw_param_blocks,
	unsigned const num_buffers,
	gfx::BufferBindingID const* const buffers
) {
	TOGO_DEBUG_ASSERTE(
		(num_draw_param_blocks == 0 || draw_param_blocks) &&
		num_buffers > 0 && buffers
	);

	{// Bind shader and validate supplied parameter blocks
	auto& shader = gfx::resource_array::get(renderer->_shaders, shader_id);
	TOGO_ASSERT(shader.id == shader_id, "invalid shader ID");
	glUseProgram(shader.handle);
	// One-time validation
	if (~shader.properties & gfx::Shader::F_VALIDATED) {
		validate_shader(renderer, shader, num_draw_param_blocks);
		shader.properties |= gfx::Shader::F_VALIDATED;
	}}

	{// Bind parameter blocks
	unsigned index = BASE_DRAW_PB_INDEX;
	for (auto const& pb_binding : array_ref(draw_param_blocks, num_draw_param_blocks)) {
		bind_param_block(
			renderer, pb_binding.id, index, pb_binding.offset, pb_binding.size
		);
		++index;
	}
	// Unbind unused indices
	unsigned const num_unbind = renderer->_num_active_draw_param_blocks;
	renderer->_num_active_draw_param_blocks = index - BASE_DRAW_PB_INDEX;
	for (; num_unbind > index; ++index) {
		unbind_param_block(renderer, index);
	}}

	{// Render!
	GLenum polygonization_method;
	unsigned index_data_type;
	for (auto const id : array_ref(buffers, num_buffers)) {
		auto const& bb = resource_array::get(renderer->_buffer_bindings, id);
		TOGO_ASSERT(bb.id == id, "invalid buffer binding ID");
		polygonization_method = gfx::g_gl_polygonization_method[
			unsigned_cast(bb.polygonization_method())
		];
		glBindVertexArray(bb.va_handle);
		if (bb.flags & gfx::BufferBinding::F_INDEXED) {
			index_data_type = bb.flags >> gfx::BufferBinding::F_SHIFT_INDEX_TYPE;
			glDrawElements(
				polygonization_method,
				bb.num_vertices,
				gfx::g_gl_primitive_type[index_data_type],
				reinterpret_cast<void const*>(
					bb.base_vertex * gfx::g_gl_primitive_size[index_data_type]
				)
			);
		} else {
			glDrawArrays(polygonization_method, bb.base_vertex, bb.num_vertices);
		}
	}}
}

void renderer::configure(
	gfx::Renderer* const renderer,
	gfx::PackedRenderConfig const& packed_config,
	Endian endian
) {
	renderer::configure_base(renderer, packed_config, endian);
}

} // namespace gfx
} // namespace game
} // namespace togo
