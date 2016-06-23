#line 2 "togo/core/memory/jump_block_allocator.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief JumpBlockAllocator class.
@ingroup lib_core_memory
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/memory/memory.hpp>

namespace togo {

/**
	@addtogroup lib_core_memory
	@{
*/

/// Cheap, non-tracking, allocator-backed allocator.
///
/// This allocator will fallback to another allocator if its block
/// does not have enough storage left to make an allocation. It is
/// intended for small, short-lifetime allocations that need to be
/// very fast. This allocator is not thread-safe.
///
/// Deallocation is a no-op; each fallback block in this allocator is
/// only freed when the object dies. Active objects will be ignored in
/// the destructor, so any resources must be explicitly
/// deallocated & destroyed before the allocator is destroyed.
///
/// This allocator does not take ownership of the base block.
///
/// Allocation works by moving a a head pointer forward in the current
/// block. If the allocation does not fit within the current block, a
/// new block is allocated. A pointer to the new block is patched into
/// the previous block by storing a next-block pointer at the front of
/// the current block (which the head pointer doesn't touch).
///
/// NB: This class does not have a virtual destructor, so deriving
/// classes must not be destroyed as-a JumpBlockAllocator.
class JumpBlockAllocator
	: public Allocator
{
private:
	char* _base_block;
	char* _block_begin;
	char* _block_end;
	char* _put;
	Allocator& _fallback_allocator;

public:
	JumpBlockAllocator(JumpBlockAllocator&&) = delete;
	JumpBlockAllocator(JumpBlockAllocator const&) = delete;
	JumpBlockAllocator& operator=(JumpBlockAllocator&&) = delete;
	JumpBlockAllocator& operator=(JumpBlockAllocator const&) = delete;

	/// Deallocates any scratch-allocated blocks.
	~JumpBlockAllocator() override;

	/// Construct with fallback allocator.
	JumpBlockAllocator(
		char* const base_block_begin,
		char* const base_block_end,
		Allocator& fallback_allocator
	);

	/// This operation is not supported.
	unsigned num_allocations() const override {
		return 0;
	}

	/// This operation is not supported.
	unsigned total_size() const override {
		return SIZE_NOT_TRACKED;
	}

	/// This operation is not supported.
	unsigned allocation_size(void const* const) const override {
		return SIZE_NOT_TRACKED;
	}

	/// Returns the next free slice.
	///
	/// If the allocation would grow past the current block, a new
	/// block will be allocated.
	void* allocate(unsigned const size, unsigned const align = DEFAULT_ALIGNMENT) override;

	/// Does nothing.
	void deallocate(void const* const) override {}
};

/** @} */ // end of doc-group lib_core_memory

} // namespace togo
