#line 2 "togo/core/memory/fixed_allocator.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief FixedAllocator class.
@ingroup lib_core_memory
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/memory/memory.hpp>

namespace togo {

/** @cond INTERNAL */

namespace memory {

void* fixed_allocator_allocate(
	u8** put_pointer,
	u8* buffer_end,
	unsigned const size,
	unsigned const align
);

}

/** @endcond */ // INTERNAL

/**
	@addtogroup lib_core_memory
	@{
*/

/// Fixed-size buffer-backed allocator.
///
/// This allocator is intended for stationary global storage.
/// It is not thread-safe.
///
/// S is the number of bytes in the buffer.
///
/// Deallocation is a no-op. Active objects will be ignored in the destructor,
/// so any resources must be explicitly deallocated & destroyed before the
/// allocator is destroyed.
template<unsigned S>
class FixedAllocator
	: public Allocator
{
public:
	static constexpr unsigned const BUFFER_SIZE = S;

	static_assert(BUFFER_SIZE > 0, "S must be greater than zero");

	u8* _put;
	u8 _buffer[BUFFER_SIZE];

	FixedAllocator(FixedAllocator&&) = delete;
	FixedAllocator(FixedAllocator const&) = delete;
	FixedAllocator& operator=(FixedAllocator&&) = delete;
	FixedAllocator& operator=(FixedAllocator const&) = delete;

	~FixedAllocator() override = default;

	/// Construct with fallback allocator.
	FixedAllocator()
		: _put(_buffer)
	{}

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
	/// If the allocation would grow past the current block, an assertion will
	/// fail.
	void* allocate(unsigned const size, unsigned const align = DEFAULT_ALIGNMENT) override {
		return memory::fixed_allocator_allocate(
			&_put, _buffer + BUFFER_SIZE,
			size, align
		);
	}

	/// Does nothing.
	void deallocate(void const* const) override {}
};

/** @} */ // end of doc-group lib_core_memory

} // namespace togo
