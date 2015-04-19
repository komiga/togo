#line 2 "togo/game/gfx/renderer/opengl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/game/gfx/types.hpp>

#include <GL/glew.h>

namespace togo {
namespace game {
namespace gfx {

enum : GLuint {
	BUFFER_HANDLE_NULL = 0,
	VERTEX_ARRAY_HANDLE_NULL = 0,
	TEXTURE_HANDLE_NULL = 0,
	RENDER_BUFFER_HANDLE_NULL = 0,
	PROGRAM_HANDLE_NULL = 0,
};

enum : GLint {
	UNIFORM_HANDLE_NULL = -1,
};

static constexpr GLenum const g_gl_buffer_data_binding[]{
	// fixed
	GL_STATIC_DRAW,
	// dynamic
	GL_DYNAMIC_DRAW,
};
static_assert(
	array_extent(g_gl_buffer_data_binding)
	== unsigned_cast(gfx::BufferDataBinding::NUM),
	""
);

static constexpr GLenum const g_gl_primitive_type[]{
	GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT,
	GL_FLOAT, GL_DOUBLE,
};
static_assert(
	array_extent(g_gl_primitive_type)
	== unsigned_cast(gfx::Primitive::NUM),
	""
);

static constexpr unsigned const g_gl_primitive_size[]{
	1, 2, 4, // u8, u16, u32
	4, 8,    // f32, f64
};
static_assert(
	array_extent(g_gl_primitive_size)
	== unsigned_cast(gfx::Primitive::NUM),
	""
);

static constexpr GLenum const g_gl_shader_stage_type[]{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
};
static_assert(
	array_extent(g_gl_shader_stage_type)
	== unsigned_cast(gfx::ShaderStage::Type::NUM),
	""
);

struct Buffer {
	enum : u32 {
		F_NONE = 0,
		F_DYNAMIC = 1 << 0,
	};

	gfx::BufferID id;
	GLuint handle;
	u32 flags;
};

struct BufferBinding {
	enum : u32 {
		F_NONE = 0,
		F_INDEXED = 1 << 0,

		// 3 bits (IndexDataType currently takes 2 at most)
		F_SHIFT_INDEX_TYPE = 29,
	};

	gfx::BufferBindingID id;
	GLuint va_handle;
	u32 base_vertex;
	u32 num_vertices;
	u32 flags;
};

struct Texture {
	gfx::TextureID id;
	GLuint handle;
};

struct RenderTarget {
	enum : u32 {
		TYPE_TEXTURE = 1 << 8,
		TYPE_BUFFER = 1 << 9,
		MASK_TYPE = TYPE_TEXTURE | TYPE_BUFFER,

		F_SWAPPED = 1 << 12,
		MASK_FLAG = F_SWAPPED,

		MASK_EXTENDED = ~gfx::RenderTargetSpec::MASK_FLAG,
	};

	enum : u32 {
		DT_DEPTH,
		DT_DEPTH_STENCIL,
		DT_COLOR,
		SHIFT_P_DATA_TYPE = 16,
		P_DATA_TYPE = (3 << SHIFT_P_DATA_TYPE),
	};

	gfx::RenderTargetID id;
	GLuint handle;
	gfx::RenderTargetSpec spec;

	bool swapped() const {
		return (spec.properties & F_SWAPPED) != 0;
	}

	unsigned data_type() const {
		return spec.properties & P_DATA_TYPE;
	}
};

#define TOGO_DT_(name) \
	(gfx::RenderTarget::DT_ ## name << gfx::RenderTarget::SHIFT_P_DATA_TYPE)
//

static constexpr struct {
	GLenum color_format;
	GLenum data_format;
	GLenum data_type;
	u32 properties;
} const g_gl_render_target_format[]{
	{GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, TOGO_DT_(DEPTH)},
	{GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, TOGO_DT_(DEPTH)},
	{GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, TOGO_DT_(DEPTH_STENCIL)},
	{GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, TOGO_DT_(COLOR)},
	{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, TOGO_DT_(COLOR)},
};

#undef TOGO_DT_

static_assert(
	array_extent(g_gl_render_target_format)
	== unsigned_cast(gfx::RenderTargetFormat::NUM),
	""
);

struct Uniform {
	gfx::UniformID id;
	GLint handle;
};

// TODO: Add param block names for validation
struct Shader {
	enum : u32 {
		// .... .... .... .... NDRA NFIX FFFF FFFF
		// FFFF = flags
		// NFIX = number of fixed param blocks
		// NDRA = number of draw param blocks

		// Whether the shader has been validated
		F_VALIDATED = 1 << 0,

		SHIFT_NUM_PB_FIXED = 8,
		SHIFT_NUM_PB_DRAW = 8 + 4,
		MASK_NUM_PB = 0x0F,
	};

	gfx::ShaderID id;
	GLuint handle;
	u32 properties;
	// NB: Currently only bits in [0, 15] are used to track fixed
	// param block use
	u32 param_block_bits;

	unsigned num_fixed_param_blocks() const {
		return (properties >> SHIFT_NUM_PB_FIXED) & MASK_NUM_PB;
	}
	unsigned num_draw_param_blocks() const {
		return (properties >> SHIFT_NUM_PB_DRAW) & MASK_NUM_PB;
	}
};

struct OpenGLRendererImpl {
	unsigned p_uniform_buffer_offset_alignment;
	unsigned p_max_uniform_block_size;
};

using RendererImpl = OpenGLRendererImpl;

} // namespace gfx
} // namespace game
} // namespace togo
