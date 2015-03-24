
#pragma once

#include <togo/memory/memory.hpp>

#if !defined(TOGO_TEST_SCRATCH_SIZE)
	#define TOGO_TEST_SCRATCH_SIZE togo::SCRATCH_ALLOCATOR_SIZE_DEFAULT
#endif

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

static void memory_init() {
	static MemoryInitializer const _ci{};
}
