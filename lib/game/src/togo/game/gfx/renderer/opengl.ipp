#line 2 "togo/game/gfx/renderer/opengl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/math.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/game/gfx/gfx/opengl_common.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/gfx/renderer/private.hpp>
#include <togo/game/gfx/renderer/opengl.hpp>

namespace togo {
namespace game {
namespace gfx {

char const* gl_get_error() {
	#define TOGO_RETURN_ERROR(err_name_) \
		case err_name_: return #err_name_;

	GLenum const error_code = glGetError();
	switch (error_code) {
	case GL_NO_ERROR: return nullptr;
	TOGO_RETURN_ERROR(GL_INVALID_ENUM)
	TOGO_RETURN_ERROR(GL_INVALID_VALUE)
	TOGO_RETURN_ERROR(GL_INVALID_OPERATION)
	TOGO_RETURN_ERROR(GL_INVALID_FRAMEBUFFER_OPERATION)
	TOGO_RETURN_ERROR(GL_OUT_OF_MEMORY)
	TOGO_RETURN_ERROR(GL_STACK_OVERFLOW)
	TOGO_RETURN_ERROR(GL_STACK_UNDERFLOW)
	default: return "UNKNOWN";
	}
	#undef TOGO_RETURN_ERROR
}

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
		TOGO_GLCE_X(glGetIntegerv(												\
			pname_,																\
			&gl_integer															\
		));																		\
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
		TOGO_GLCE_X(glBindBuffer(target, buffer.handle));
	} else {
		TOGO_GLCE_X(glBindBuffer(target, BUFFER_HANDLE_NULL));
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
	TOGO_GLCE_X(glBindBufferRange(
		GL_UNIFORM_BUFFER, index, buffer.handle, offset, size
	));
}

inline static void unbind_param_block(
	gfx::Renderer* const /*renderer*/,
	unsigned const index
) {
	TOGO_GLCE_X(glBindBufferBase(GL_UNIFORM_BUFFER, index, BUFFER_HANDLE_NULL));
}

inline static void setup_param_block_bindings(
	gfx::Shader const& shader,
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND> const& param_blocks,
	unsigned const base_buffer_index
) {
	GLuint block_index{GL_INVALID_INDEX};
	for (auto const& pb_def : param_blocks) {
		TOGO_GLCE_X(block_index = glGetUniformBlockIndex(shader.handle, pb_def.name.data));
		TOGO_ASSERTE(block_index != GL_INVALID_INDEX);
		TOGO_GLCE_X(glUniformBlockBinding(
			shader.handle, block_index, base_buffer_index + pb_def.index
		));
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
	TOGO_GLCE_X(glValidateProgram(shader.handle));
	TOGO_GLCE_X(glGetProgramiv(shader.handle, GL_VALIDATE_STATUS, &status));
	TOGO_GLCE_X(glGetProgramInfoLog(shader.handle, array_extent(info_log), &log_size, info_log));

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
	TOGO_GLCE_X(glEnable(GL_DEPTH_TEST));
	TOGO_GLCE_X(glDepthFunc(GL_ALWAYS));

	GLint vp[4];
	TOGO_GLCE_X(glGetIntegerv(GL_VIEWPORT, vp));
	renderer->_viewport_size.x = unsigned_cast(max(0, vp[2]));
	renderer->_viewport_size.y = unsigned_cast(max(0, vp[3]));
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
	TOGO_GLCE_X(glGenBuffers(1, &buffer.handle));
	TOGO_ASSERTE(buffer.handle != BUFFER_HANDLE_NULL);
	TOGO_GLCE_X(glBindBuffer(GL_ARRAY_BUFFER, buffer.handle));
	TOGO_GLCE_X(glBufferData(
		GL_ARRAY_BUFFER,
		size, data, gfx::g_gl_buffer_data_binding[unsigned_cast(data_binding)]
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
			|= gfx::BufferBinding::F_INDEXED
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
	auto const type
		= (
			format_info.requires_texture ||
			spec.properties & gfx::RenderTargetSpec::F_DOUBLE_BUFFERED
		)
		? gfx::RenderTarget::TYPE_TEXTURE
		: gfx::RenderTarget::TYPE_BUFFER
	;
	spec.properties &= ~gfx::RenderTarget::MASK_TYPE;
	spec.properties |= type;

	Vec2 size{spec.dim_x, spec.dim_y};
	if (spec.properties & gfx::RenderTargetSpec::F_SCALE) {
		size *= renderer->_viewport_size;
	}

	if (type == gfx::RenderTarget::TYPE_TEXTURE) {
		TOGO_GLCE_X(glGenTextures(1, &render_target.handle));
		TOGO_ASSERTE(render_target.handle != TEXTURE_HANDLE_NULL);

		GLenum target
			= spec.properties & gfx::RenderTargetSpec::F_DOUBLE_BUFFERED
			? GL_TEXTURE_2D_ARRAY
			: GL_TEXTURE_2D
		;
		TOGO_GLCE_X(glBindTexture(target, render_target.handle));
		TOGO_GLCE_X(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		TOGO_GLCE_X(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		TOGO_GLCE_X(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		TOGO_GLCE_X(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		if (target == GL_TEXTURE_2D) {
			TOGO_GLCE_X(glTexImage2D(
				target, 0,
				format_info.color_format,
				size.x, size.y, 0,
				format_info.data_format,
				format_info.data_type,
				nullptr
			));
		} else {
			TOGO_GLCE_X(glTexImage3D(
				target, 0,
				format_info.color_format,
				size.x, size.y, 2, 0,
				format_info.data_format,
				format_info.data_type,
				nullptr
			));
		}
		TOGO_GLCE_X(glBindTexture(target, TEXTURE_HANDLE_NULL));
	} else if (type == gfx::RenderTarget::TYPE_BUFFER) {
		TOGO_GLCE_X(glGenRenderbuffers(1, &render_target.handle));
		TOGO_ASSERTE(render_target.handle != RENDER_BUFFER_HANDLE_NULL);
		TOGO_GLCE_X(glBindRenderbuffer(GL_RENDERBUFFER, render_target.handle));
		TOGO_GLCE_X(glRenderbufferStorage(
			GL_RENDERBUFFER,
			format_info.color_format,
			size.x, size.y
		));
		TOGO_GLCE_X(glBindRenderbuffer(GL_RENDERBUFFER, RENDER_BUFFER_HANDLE_NULL));
	}
	return resource_array::assign(renderer->_render_targets, render_target).id;
}

void renderer::destroy_render_target(
	gfx::Renderer* const renderer,
	gfx::RenderTargetID const id
) {
	auto& render_target = resource_array::get(renderer->_render_targets, id);
	TOGO_ASSERT(render_target.id == id, "invalid render_target ID");

	auto const& spec = render_target.spec;
	switch (spec.properties & gfx::RenderTarget::MASK_TYPE) {
	case gfx::RenderTarget::TYPE_TEXTURE:
		TOGO_GLCE_X(glDeleteTextures(1, &render_target.handle));
		break;

	case gfx::RenderTarget::TYPE_BUFFER:
		TOGO_GLCE_X(glDeleteRenderbuffers(1, &render_target.handle));
		break;

	default:
		TOGO_DEBUG_ASSERTE(false);
		break;
	}
	resource_array::free(renderer->_render_targets, render_target);
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
		for (StringRef const& stage_source : stage.sources) {
			fixed_array::push_back(sources, stage_source.data);
			fixed_array::push_back(sources_size, signed_cast(stage_source.size));
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
		TOGO_GLCE_X(glGetShaderInfoLog(shader_handle, array_extent(info_log), &log_size, info_log));
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
	gfx::Shader shader{{}, PROGRAM_HANDLE_NULL, 0, 0};
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
	TOGO_GLCE_X(glGetProgramInfoLog(shader.handle, array_extent(info_log), &log_size, info_log));
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
	TOGO_GLCE_X(glDeleteProgram(shader.handle));
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
	TOGO_GLCE_X(glViewport(0, 0, size.x, size.y));
	renderer->_viewport_size = size;
}

void renderer::clear_backbuffer(
	gfx::Renderer* const /*renderer*/
) {
	Vec4 const clear_color{0.0f, 0.0f, 0.0f, 1.0f};
	TOGO_GLCE_X(glClearBufferfv(GL_COLOR, 0, &clear_color.x));
	TOGO_GLCE_X(glClearBufferfi(GL_DEPTH_STENCIL, 0, 0.0f, 0));
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
	TOGO_GLCE_X(glUseProgram(shader.handle));
	// One-time validation
	if (~shader.properties & gfx::Shader::F_VALIDATED) {
		validate_shader(renderer, shader, num_draw_param_blocks);
		shader.properties |= gfx::Shader::F_VALIDATED;
	}}

	{// Bind parameter blocks
	unsigned index = BASE_DRAW_PB_INDEX;
	for (auto const& pb_binding : array_ref(num_draw_param_blocks, draw_param_blocks)) {
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
	unsigned index_data_type;
	for (auto const id : array_ref(num_buffers, buffers)) {
		auto const& bb = resource_array::get(renderer->_buffer_bindings, id);
		TOGO_ASSERT(bb.id == id, "invalid buffer binding ID");
		TOGO_GLCE_X(glBindVertexArray(bb.va_handle));
		if (bb.flags & gfx::BufferBinding::F_INDEXED) {
			index_data_type = bb.flags >> gfx::BufferBinding::F_SHIFT_INDEX_TYPE;
			glDrawElements(
				GL_TRIANGLES,
				bb.num_vertices,
				gfx::g_gl_primitive_type[index_data_type],
				reinterpret_cast<void const*>(
					bb.base_vertex * gfx::g_gl_primitive_size[index_data_type]
				)
			);
		} else {
			glDrawArrays(GL_TRIANGLES, bb.base_vertex, bb.num_vertices);
		}
	}}
}

void renderer::configure(
	gfx::Renderer* const renderer,
	gfx::RenderConfig const& config
) {
	renderer::configure_base(renderer, config);
	// TODO: Create global resources
}

} // namespace gfx
} // namespace game
} // namespace togo
