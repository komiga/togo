#line 2 "togo/core/memory/temp_allocator.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief TempAllocator class.
@ingroup lib_core_memory
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/jump_block_allocator.hpp>

namespace togo {

/**
	@addtogroup lib_core_memory
	@{
*/

/// Temporary stack-buffered, allocator-backed allocator.
///
/// This allocator is backed by a JumpBlockAllocator with an internal
/// char[S] buffer as the base block. This allocator is not
/// thread-safe.
///
/// S is the number of bytes to store in the allocator directly.
/// S must be a power of 2 at least twice the size of a pointer.
template<unsigned S>
class TempAllocator
	: public JumpBlockAllocator
{
private:
	static constexpr unsigned const
	BUFFER_SIZE = S;

	static_assert(
		(BUFFER_SIZE & (BUFFER_SIZE - 1)) == 0 &&
		BUFFER_SIZE >= 2 * sizeof(void*),
		"S must be a power of 2 at least twice the size of a pointer"
	);

	char _buffer[BUFFER_SIZE];

public:
	TempAllocator(TempAllocator&&) = delete;
	TempAllocator(TempAllocator const&) = delete;
	TempAllocator& operator=(TempAllocator&&) = delete;
	TempAllocator& operator=(TempAllocator const&) = delete;

	~TempAllocator() = default;

	/// Construct with fallback allocator.
	TempAllocator(Allocator& fallback_allocator = memory::scratch_allocator())
		: JumpBlockAllocator(
			_buffer,
			_buffer + BUFFER_SIZE,
			fallback_allocator
		)
	{}
};

/** @} */ // end of doc-group lib_core_memory

} // namespace togo
