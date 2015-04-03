#line 2 "togo/game/gfx/display.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Graphics display interface.
@ingroup lib_game_gfx
@ingroup lib_game_gfx_display

@defgroup lib_game_gfx_display Display
@ingroup lib_game_gfx
@details
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/display.gen_interface>

namespace togo {
namespace gfx {
namespace display {

/**
	@addtogroup lib_game_gfx_display
	@{
*/

// implementation

/// Create graphics display.
///
/// An assertion will fail if the display could not be created.
/// The new display's context will be current.
/// If share_with is non-null, the new display will share its context.
/// If gfx::DisplayFlags::fullscreen is enabled, width and height
/// are ignored.
gfx::Display* create(
	char const* title,
	unsigned width,
	unsigned height,
	gfx::DisplayFlags flags,
	gfx::DisplayConfig const& config,
	gfx::Display* share_with = nullptr,
	Allocator& allocator = memory::default_allocator()
);

/// Set title.
void set_title(gfx::Display* display, char const* title);

/// Set mouse input lock.
///
/// If this is enabled, the mouse position will be locked to the
/// display window.
void set_mouse_lock(gfx::Display* display, bool enable);

/// Set swap mode.
///
/// @warning This applies to the display's context, not individual
/// displays. The display's context will be current after this call.
void set_swap_mode(gfx::Display* display, gfx::DisplaySwapMode mode);

/// Bind the display context to the current thread.
///
/// Only one thread at a time can have a display's context bound.
void bind_context(gfx::Display* display);

/// Unbind the current display context from the current thread.
void unbind_context();

/// Swap buffers in display.
///
/// This does nothing if the display is not double-buffered.
void swap_buffers(gfx::Display* display);

/// Destroy graphics display.
void destroy(gfx::Display* display);

/** @} */ // end of doc-group lib_game_gfx_display

} // namespace display
} // namespace gfx
} // namespace togo
