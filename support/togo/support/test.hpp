#line 2 "togo/support/test.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Test support.
@ingroup support_test

@defgroup support_test Test
@ingroup support
@details
*/

#pragma once

#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>

/**
	@addtogroup support_test
	@{
*/

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// The scratch size to init with.
	///
	/// Defaults to togo::SCRATCH_ALLOCATOR_SIZE_DEFAULT.
	#define TOGO_SUPPORT_MEMINIT_SCRATCH_SIZE
#elif !defined(TOGO_SUPPORT_MEMINIT_SCRATCH_SIZE)
	#define TOGO_SUPPORT_MEMINIT_SCRATCH_SIZE togo::SCRATCH_ALLOCATOR_SIZE_DEFAULT
#endif

/// Memory system initializer.
///
/// Calls togo::memory::init() on construction and togo::memory::shutdown()
/// on destruction.
struct MemoryInitializer {
	MemoryInitializer(MemoryInitializer const&) = delete;
	MemoryInitializer& operator=(MemoryInitializer const&) = delete;
	MemoryInitializer(MemoryInitializer&&) = delete;
	MemoryInitializer& operator=(MemoryInitializer&&) = delete;

	MemoryInitializer() {
		togo::memory::init(TOGO_SUPPORT_MEMINIT_SCRATCH_SIZE);
	}

	~MemoryInitializer() {
		togo::memory::shutdown();
	}
};

/// Ensure initialization of a static MemoryInitializer.
void memory_init() {
	static MemoryInitializer const _ci{};
}

/// Assert if an expression does not abort.
#define TOGO_TEST_SHOULD_FAIL(expr_) do { \
	TOGO_LOG_DEBUG("TOGO_TEST_SHOULD_FAIL: `" #expr_ "`\n"); \
	(expr_); \
	TOGO_ASSERT(false, "statement that should've failed did not"); \
} while (false)

/** @} */ // end of doc-group support_test
