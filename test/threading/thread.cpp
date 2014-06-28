
#include <togo/assert.hpp>
#include <togo/thread.hpp>

#include "../common/helpers.hpp"

#include <cstdio>

using namespace togo;

static int static_rv_check = 0;

void* thread_func(void* i_void) {
	int const* const value = static_cast<int*>(i_void);
	std::printf("thread_func: '%s', %d\n", thread::name(), *value);
	static_rv_check = *value;
	return &static_rv_check;
}

signed
main() {
	core_init();

	std::printf("main thread: '%s'\n", thread::name());

	int call_data = 42;
	Thread* t = thread::create("thread-1", &call_data, thread_func);
	TOGO_ASSERTE(t);
	std::printf("new thread: '%s'\n", thread::name(t));

	void* exit_value = thread::join(t);
	TOGO_ASSERTE(exit_value == &static_rv_check && static_rv_check == call_data);
	return 0;
}
