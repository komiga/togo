#line 2 "togo/window/window/impl/opengl.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/window/window/impl/types.hpp>
#include <togo/window/opengl.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <glad/glad.c>
#pragma GCC diagnostic pop

namespace togo {
namespace window {

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

inline static void gl_set_cap_active(GLenum cap, bool enable) {
	if (enable) {
		glEnable(cap);
	} else {
		glDisable(cap);
	}
}

struct NamedEnum {
	StringRef name;
	GLenum value;
};

NamedEnum const
#define TOGO_NAMED_ENUM(name, value) {name, GL_DEBUG_ ## value},
gl_enum_debug_source[]{
	TOGO_NAMED_ENUM("api" , SOURCE_API)
	TOGO_NAMED_ENUM("wsys", SOURCE_WINDOW_SYSTEM)
	TOGO_NAMED_ENUM("scc" , SOURCE_SHADER_COMPILER)
	TOGO_NAMED_ENUM("3rd" , SOURCE_THIRD_PARTY)
	TOGO_NAMED_ENUM("user", SOURCE_APPLICATION)
	TOGO_NAMED_ENUM("etc" , SOURCE_OTHER)
},
gl_enum_debug_type[]{
	TOGO_NAMED_ENUM("error"     , TYPE_ERROR)
	TOGO_NAMED_ENUM("deprecated", TYPE_DEPRECATED_BEHAVIOR)
	TOGO_NAMED_ENUM("undef"     , TYPE_UNDEFINED_BEHAVIOR)
	TOGO_NAMED_ENUM("port"      , TYPE_PORTABILITY)
	TOGO_NAMED_ENUM("perf"      , TYPE_PERFORMANCE)
	TOGO_NAMED_ENUM("etc"       , TYPE_OTHER)
	TOGO_NAMED_ENUM("marker"    , TYPE_MARKER)
	TOGO_NAMED_ENUM("push"      , TYPE_PUSH_GROUP)
	TOGO_NAMED_ENUM("pop"       , TYPE_POP_GROUP)
},
gl_enum_debug_severity[]{
	TOGO_NAMED_ENUM("fyi" , SEVERITY_NOTIFICATION)
	TOGO_NAMED_ENUM("low" , SEVERITY_LOW)
	TOGO_NAMED_ENUM("med" , SEVERITY_MEDIUM)
	TOGO_NAMED_ENUM("high", SEVERITY_HIGH)
}
#undef TOGO_NAMED_ENUM
;

inline static StringRef get_enum_name(
	ArrayRef<NamedEnum const> e,
	GLenum value
) {
	for (auto& pair : e) {
		if (pair.value == value) {
			return pair.name;
		}
	}
	return "???";
}

static void gl_debug_message_callback(
	GLenum source,
	GLenum type,
	GLuint /*id*/,
	GLenum severity,
	GLsizei length,
	GLchar const* message_data,
	void const* /*userParam*/
) {
	auto severity_str = get_enum_name(gl_enum_debug_severity, severity);
	auto source_str = get_enum_name(gl_enum_debug_source, source);
	auto type_str = get_enum_name(gl_enum_debug_type, type);
	StringRef message{message_data, unsigned_cast(length)};
	if (message.size > 0 && message[message.size - 1] == '\n') {
		--message.size;
	}
	TOGO_LOGF(
		"OpenGL: debug: [%-4.*s] [%-4.*s] [%-10.*s] %.*s\n",
		severity_str.size, severity_str.data,
		source_str.size, source_str.data,
		type_str.size, type_str.data,
		message.size, message.data
	);
}

static bool set_opengl_debug_mode_impl(bool active) {
	if (!_globals.opengl_initialized) {
		return false;
	}
	if (GLAD_GL_KHR_debug) {
		if (active) {
			TOGO_GLCE(gl_set_cap_active(GL_DEBUG_OUTPUT, true));
			TOGO_GLCE(gl_set_cap_active(GL_DEBUG_OUTPUT_SYNCHRONOUS, true));
			TOGO_GLCE(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true));
			TOGO_GLCE(glDebugMessageCallback(gl_debug_message_callback, nullptr));
			TOGO_LOG("OpenGL: debug enabled\n");
		} else if (glIsEnabled(GL_DEBUG_OUTPUT)) {
			TOGO_GLCE(glDebugMessageCallback(nullptr, nullptr));
			TOGO_GLCE(gl_set_cap_active(GL_DEBUG_OUTPUT_SYNCHRONOUS, false));
			TOGO_GLCE(gl_set_cap_active(GL_DEBUG_OUTPUT, false));
			TOGO_LOG("OpenGL: debug disabled\n");
		}
		return true;
	} else if (active) {
		TOGO_LOG_ERROR("GL_KHR_debug extension not supported\n");
	}
	return !active;
}

inline static void log_gl_string(StringRef name, GLenum id) {
	auto value = reinterpret_cast<char const*>(glGetString(id));
	TOGO_LOGF("  %.*s = %s\n", name.size, name.data, value ? value : "(null)");
}

void init_opengl() {
	if (_globals.opengl_initialized) {
		return;
	}

	TOGO_ASSERT(gladLoadGL(), "failed to initialize OpenGL");

	#define LOG_GL_STRING(name) \
		log_gl_string(#name, name)

	TOGO_LOG("OpenGL context properties:\n");
	LOG_GL_STRING(GL_VENDOR);
	LOG_GL_STRING(GL_VERSION);
	LOG_GL_STRING(GL_SHADING_LANGUAGE_VERSION);
	LOG_GL_STRING(GL_RENDERER);

	#undef LOG_GL_STRING

	#define LOG_GL_FLAG(flag) \
		if (context_flags & (GL_CONTEXT_FLAG_ ## flag ## _BIT)) TOGO_LOG("  " #flag);

	{GLint context_flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	TOGO_LOG("  GL_CONTEXT_FLAGS:");
		LOG_GL_FLAG(FORWARD_COMPATIBLE)
		LOG_GL_FLAG(DEBUG)
	TOGO_LOG("\n");
	}

	#undef LOG_GL_FLAG

	#if defined(TOGO_DEBUG)
	{GLint num = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);
	TOGO_LOGF("OpenGL extensions (%d):\n", num);
	for (signed i = 0; i < num; ++i) {
		auto value = glGetStringi(GL_EXTENSIONS, i);
		if (value) {
			TOGO_LOGF("    %s\n", value);
		}
	}}
	#endif

	_globals.opengl_initialized = true;
	set_opengl_debug_mode_impl(_globals.opengl_debug);
}

bool set_opengl_debug_mode(bool active) {
	if (active == _globals.opengl_debug) {
		return true;
	}
	_globals.opengl_debug = active;
	if (!_globals.opengl_initialized) {
		return true;
	}
	return set_opengl_debug_mode_impl(_globals.opengl_debug);
}

} // namespace window
} // namespace togo
