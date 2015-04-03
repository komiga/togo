#line 2 "togo/game/gfx/display/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/input/types.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/game/gfx/display/sdl.hpp>
#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
	#include <togo/game/gfx/display/glfw.hpp>
#endif

namespace togo {
namespace gfx {

struct Display {
	unsigned _width;
	unsigned _height;
	gfx::DisplayFlags _flags;
	gfx::DisplayConfig _config;
	Allocator* _allocator;
	InputBuffer* _input_buffer;

	FixedArray<KeyCode, 32> _key_clear_queue;
	u8 _key_states[static_cast<unsigned>(KeyCode::COUNT)];
	u8 _mouse_button_states[static_cast<unsigned>(MouseButton::COUNT)];
	signed _mouse_x, _mouse_y;

	DisplayImpl _impl;

	Display(Display&&) = default;
	Display& operator=(Display&&) = default;

	Display() = delete;
	Display(Display const&) = delete;
	Display& operator=(Display const&) = delete;

	~Display();
	Display(
		unsigned const width,
		unsigned const height,
		gfx::DisplayFlags const flags,
		DisplayConfig const& config,
		Allocator& allocator,
		DisplayImpl&& impl
	)
		: _width(width)
		, _height(height)
		, _flags(flags)
		, _config(config)
		, _allocator(&allocator)
		, _input_buffer(nullptr)
		, _key_clear_queue()
		, _key_states()
		, _mouse_button_states()
		, _mouse_x(0)
		, _mouse_y(0)
		, _impl(impl)
	{}
};

} // namespace gfx
} // namespace togo