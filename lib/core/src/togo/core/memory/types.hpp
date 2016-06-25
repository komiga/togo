#line 2 "togo/core/memory/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Memory types.
@ingroup lib_core_types
@ingroup lib_core_memory
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_core_memory
	@{
*/

// Forward declarations
class Allocator;
class AssertAllocator;
template<unsigned S>
class FixedAllocator;
class JumpBlockAllocator;
template<unsigned S>
class TempAllocator;

/// Memory constants.
enum : unsigned {
	/// Minimum scratch size (8K).
	SCRATCH_ALLOCATOR_SIZE_MINIMUM = 8 * 1024,
	/// Default scratch size (4MB).
	SCRATCH_ALLOCATOR_SIZE_DEFAULT = 4 * 1024 * 1024
};

/** @} */ // end of doc-group lib_core_memory

} // namespace togo
