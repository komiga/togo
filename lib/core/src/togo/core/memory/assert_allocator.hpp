#line 2 "togo/core/memory/assert_allocator.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief AssertAllocator class.
@ingroup lib_core_memory
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/memory/memory.hpp>

namespace togo {

/**
	@addtogroup lib_core_memory
	@{
*/

/// Allocator that always fails.
class AssertAllocator
	: public Allocator
{
public:
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

	/// Fails with an assertion.
	[[noreturn]]
	void* allocate(unsigned const, unsigned const = DEFAULT_ALIGNMENT) override {
		TOGO_ASSERTE(false);
	}

	/// Fails with an assertion if p != nullptr.
	void deallocate(void const* const p) override {
		TOGO_ASSERTE(!p);
	}
};

/** @} */ // end of doc-group lib_core_memory

} // namespace togo
