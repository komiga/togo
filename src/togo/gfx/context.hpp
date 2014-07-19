#line 2 "togo/gfx/context.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file gfx/context.hpp
@brief Graphics context interface.
@ingroup gfx
@ingroup gfx_context
*/

#pragma once

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/gfx/types.hpp>
#include <togo/memory.hpp>

namespace togo {
namespace gfx {
namespace context {

/**
	@addtogroup gfx
	@{
*/
/**
	@addtogroup gfx_context
	@{
*/

/// Create graphics context.
///
/// An assertion will fail if the context could not be created.
/// If gfx::ContextFlags::shared is enabled, the new context will be
/// shared with the current context for the display.
gfx::Context* create(
	gfx::Display* display,
	gfx::ContextFlags flags,
	Allocator& allocator = memory::default_allocator()
);

/// Destroy graphics context.
void destroy(gfx::Context* context);

/** @} */ // end of doc-group gfx_context
/** @} */ // end of doc-group gfx

} // namespace context
} // namespace gfx
} // namespace togo
