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

enum class GLBufferFlags : unsigned {
	none = 0,
	dynamic = 1 << 0,
};

struct Buffer {
	gfx::BufferID id;
	GLuint handle;
	gfx::GLBufferFlags flags;
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
