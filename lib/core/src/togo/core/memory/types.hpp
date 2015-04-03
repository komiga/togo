#line 2 "togo/core/memory/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Memory types.
@ingroup types
@ingroup memory
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup memory
	@{
*/

// Forward declarations
class Allocator;

/// Memory constants.
enum : unsigned {
	/// Minimum scratch size (8K).
	SCRATCH_ALLOCATOR_SIZE_MINIMUM = 8 * 1024,
	/// Default scratch size (4MB).
	SCRATCH_ALLOCATOR_SIZE_DEFAULT = 4 * 1024 * 1024
};

/** @} */ // end of doc-group memory

} // namespace togo
