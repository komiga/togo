#line 2 "togo/impl/gfx/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/memory_types.hpp>
#include <togo/gfx/types.hpp>
#include <togo/input_types.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/impl/gfx/display/sdl.hpp>
#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
	#include <togo/impl/gfx/display/glfw.hpp>
#endif

namespace togo {
namespace gfx {

struct Display {
	unsigned _width;
	unsigned _height;
	gfx::DisplayFlags _flags;
	gfx::Config _config;
	Allocator* _allocator;
	InputBuffer* _input_buffer;
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
		Config const& config,
		Allocator& allocator,
		DisplayImpl&& impl
	)
		: _width(width)
		, _height(height)
		, _flags(flags)
		, _config(config)
		, _allocator(&allocator)
		, _input_buffer(nullptr)
		, _impl(impl)
	{}
};

} // namespace gfx
} // namespace togo
