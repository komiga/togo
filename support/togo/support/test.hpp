#line 2 "togo/support/test.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Test support.
@ingroup support
*/

#pragma once

#include <togo/core/memory/memory.hpp>

#if !defined(TOGO_TEST_SCRATCH_SIZE)
	#define TOGO_TEST_SCRATCH_SIZE togo::SCRATCH_ALLOCATOR_SIZE_DEFAULT
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
		togo::memory::init(TOGO_TEST_SCRATCH_SIZE);
	}

	~MemoryInitializer() {
		togo::memory::shutdown();
	}
};

/// Ensure initialization of a static MemoryInitializer.
static void memory_init() {
	static MemoryInitializer const _ci{};
}
