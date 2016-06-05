#line 2 "togo/window/window/impl/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/window/window/types.hpp>
#include <togo/window/input/types.hpp>

#if defined(TOGO_WINDOW_BACKEND_SUPPORTS_OPENGL)
	#include <togo/window/window/impl/opengl.hpp>
#endif

#if (TOGO_CONFIG_WINDOW_BACKEND == TOGO_WINDOW_BACKEND_SDL)
	#include <togo/window/window/impl/sdl.hpp>
#elif (TOGO_CONFIG_WINDOW_BACKEND == TOGO_WINDOW_BACKEND_GLFW)
	#include <togo/window/window/impl/glfw.hpp>
#endif

namespace togo {

namespace window {

struct Globals {
	bool initialized;
	bool opengl_initialized;
	unsigned context_major;
	unsigned context_minor;
};

extern Globals _globals;

} // namespace window

struct Window {
	UVec2 _size;
	WindowFlags _flags;
	WindowOpenGLConfig _config;
	Allocator* _allocator;
	InputBuffer* _input_buffer;

	FixedArray<KeyCode, 32> _key_clear_queue;
	u8 _key_states[static_cast<unsigned>(KeyCode::COUNT)];
	u8 _mouse_button_states[static_cast<unsigned>(MouseButton::COUNT)];
	signed _mouse_x, _mouse_y;

	WindowImpl _impl;

	Window() = delete;
	Window(Window const&) = delete;
	Window& operator=(Window const&) = delete;

	Window(Window&&) = default;
	Window& operator=(Window&&) = default;

	~Window();
	Window(
		UVec2 const size,
		WindowFlags const flags,
		WindowOpenGLConfig const& config,
		Allocator& allocator,
		WindowImpl&& impl
	)
		: _size(size)
		, _flags(flags)
		, _config(config)
		, _allocator(&allocator)
		, _input_buffer(nullptr)
		, _key_clear_queue()
		, _key_states()
		, _mouse_button_states()
		, _mouse_x(0)
		, _mouse_y(0)
		, _impl(rvalue_ref(impl))
	{}
};

} // namespace togo
