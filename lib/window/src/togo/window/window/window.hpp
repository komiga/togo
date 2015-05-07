#line 2 "togo/window/window/window.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Window interface.
@ingroup lib_window_window
*/

#pragma once

#include <togo/window/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/string/types.hpp>
#include <togo/window/window/types.hpp>
#include <togo/window/window/window.gen_interface>

namespace togo {
namespace window {

/**
	@addtogroup lib_window_window
	@{
*/

// implementation

/// Create raster window.
///
/// An assertion will fail if the window could not be created.
/// If WindowFlags::fullscreen is enabled, size is ignored.
Window* create_raster(
	StringRef title,
	UVec2 size,
	WindowFlags flags,
	Allocator& allocator = memory::default_allocator()
);

/// Create window with OpenGL context.
///
/// An assertion will fail if the window could not be created.
/// The new window's OpenGL context will be current.
/// If share_with is non-null, the new window will share its OpenGL context.
/// If WindowFlags::fullscreen is enabled, size is ignored.
Window* create_opengl(
	StringRef title,
	UVec2 size,
	WindowFlags flags,
	WindowOpenGLConfig const& config,
	Window* share_with = nullptr,
	Allocator& allocator = memory::default_allocator()
);

/// Destroy window.
void destroy(Window* window);

/// Set title.
void set_title(Window* window, StringRef title);

/// Set mouse input lock.
///
/// If this is enabled, the mouse position will be locked to the
/// window.
void set_mouse_lock(Window* window, bool enable);

/// Set swap mode.
///
/// @warning This applies to the window's OpenGL context, not individual
/// windows. The window's OpenGL context will be current after this call.
void set_swap_mode(Window* window, WindowSwapMode mode);

/// Bind the window context to the current thread.
///
/// Only one thread at a time can have a window's OpenGL context bound.
void bind_context(Window* window);

/// Unbind the current window OpenGL context from the current thread.
void unbind_context();

/// Swap buffers in window.
///
/// This does nothing if the window is not double-buffered.
void swap_buffers(Window* window);

/** @} */ // end of doc-group lib_window_window

} // namespace window
} // namespace togo
