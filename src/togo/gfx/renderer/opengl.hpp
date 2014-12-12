#line 2 "togo/gfx/renderer/opengl.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/utility/utility.hpp>
#include <togo/log/log.hpp>
#include <togo/gfx/types.hpp>

#include <GL/glew.h>

namespace togo {
namespace gfx {

char const* gl_get_error();

#define TOGO_GLCE() \
	do { while (char const* gl_error__ = gfx::gl_get_error()) { \
		TOGO_LOG_ERRORF("OpenGL error: %s\n", gl_error__); \
	} } while (false)

#define TOGO_GLCE_X(expr_) do { (expr_); TOGO_GLCE(); } while (false)

enum : GLuint {
	BUFFER_HANDLE_NULL = 0,
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

enum class GLBufferFlags : unsigned {
	none = 0,
	dynamic = 1 << 0,
};

struct VertexBuffer {
	gfx::VertexBufferID id;
	GLuint handle;
	gfx::GLBufferFlags flags;
	gfx::VertexFormat const* format;
};

struct IndexBuffer {
	gfx::IndexBufferID id;
	GLuint handle;
	gfx::GLBufferFlags flags;
};

struct Texture {
	gfx::TextureID id;
	GLuint handle;
};

struct Uniform {
	gfx::UniformID id;
	GLint handle;
};

struct Shader {
	gfx::ShaderID id;
	GLuint handle;
};

struct OpenGLRendererImpl {};

using RendererImpl = OpenGLRendererImpl;

} // namespace gfx
} // namespace togo
