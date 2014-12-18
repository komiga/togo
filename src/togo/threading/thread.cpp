#line 2 "togo/threading/thread.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/memory/types.hpp>
#include <togo/threading/types.hpp>
#include <togo/threading/thread.hpp>

#include <cstring>

namespace togo {

enum : u32 {
	THREAD_DATA_NAME_SIZE = 32
};

struct ThreadData {
	void* call_data;
	void* (*func)(void* call_data);
	Allocator* allocator;
	char name[THREAD_DATA_NAME_SIZE];
};

inline void thread_data_init(
	ThreadData& data,
	char const* name,
	void* call_data,
	void* (*func)(void* call_data),
	Allocator& allocator
) {
	TOGO_DEBUG_ASSERTE(name);
	TOGO_DEBUG_ASSERTE(func);
	std::strncpy(data.name, name, THREAD_DATA_NAME_SIZE - 1);
	data.name[THREAD_DATA_NAME_SIZE - 1] = '\0';
	data.call_data = call_data;
	data.func = func;
	data.allocator = &allocator;
}

namespace {
	struct ThreadLocalRef {
		char const* name;
	};

	thread_local ThreadLocalRef
	_tls_thread_ref{
		nullptr
	};
} // anonymous namespace

inline void* thread_runner(ThreadData& data) {
	_tls_thread_ref.name = data.name;
	void* exit_value = data.func(data.call_data);
	_tls_thread_ref.name = nullptr;
	return exit_value;
}

char const* thread::name() {
	if (thread::is_main()) {
		return "main";
	} else if (!_tls_thread_ref.name) {
		return "uninitialized-thread";
	} else {
		return _tls_thread_ref.name;
	}
}

} // namespace togo

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/threading/thread/posix.ipp>
#else
	#error "missing thread implementation for target platform"
#endif
