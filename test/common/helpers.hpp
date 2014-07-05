
#pragma once

#include <togo/memory.hpp>

#if !defined(TOGO_TEST_SCRATCH_SIZE)
	#define TOGO_TEST_SCRATCH_SIZE togo::memory::SCRATCH_SIZE_DEFAULT
#endif

struct CoreInitializer {
	CoreInitializer(CoreInitializer const&) = delete;
	CoreInitializer& operator=(CoreInitializer const&) = delete;
	CoreInitializer(CoreInitializer&&) = delete;
	CoreInitializer& operator=(CoreInitializer&&) = delete;

	CoreInitializer() {
		togo::memory::init(TOGO_TEST_SCRATCH_SIZE);
	}

	~CoreInitializer() {
		togo::memory::shutdown();
	}
};

static void core_init() {
	static CoreInitializer const _ci{};
}
