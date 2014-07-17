#line 2 "togo/impl/gfx/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/memory_types.hpp>
#include <togo/gfx/types.hpp>

#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
	#include <togo/impl/gfx/display/sdl.hpp>
	#include <togo/impl/gfx/context/sdl.hpp>
#endif

namespace togo {
namespace gfx {

struct Display {
	unsigned _width;
	unsigned _height;
	gfx::DisplayFlags _flags;
	gfx::Config _config;
	Allocator* _allocator;
	DisplayImpl _impl;

	~Display() = default;
	Display(Display&&) = default;
	Display& operator=(Display&&) = default;

	Display() = delete;
	Display(Display const&) = delete;
	Display& operator=(Display const&) = delete;

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
		, _impl(impl)
	{}
};

struct Context {
	ContextFlags _flags;
	Allocator* _allocator;
	ContextImpl _impl;

	~Context() = default;
	Context(Context&&) = default;
	Context& operator=(Context&&) = default;

	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;

	Context(
		ContextFlags const flags,
		Allocator& allocator,
		ContextImpl&& impl
	)
		: _flags(flags)
		, _allocator(&allocator)
		, _impl(impl)
	{}
};

} // namespace gfx
} // namespace togo
