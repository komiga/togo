#line 2 "togo/gfx/gfx.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Graphics interface.
@ingroup gfx
*/

#pragma once

#include <togo/config.hpp>
#include <togo/gfx/types.hpp>

namespace togo {
namespace gfx {

/**
	@addtogroup gfx
	@{
*/

/// Initialize the graphics backend.
///
/// context_major and context_minor are OpenGL context version numbers.
/// If these are less than 3 and 2, respectively, an assertion will fail.
/// The core profile is forced.
void init(
	unsigned context_major,
	unsigned context_minor
);

/// Shutdown the graphics backend.
void shutdown();

/** @} */ // end of doc-group gfx

} // namespace gfx
} // namespace togo
