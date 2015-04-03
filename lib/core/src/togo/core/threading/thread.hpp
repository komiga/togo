#line 2 "togo/core/threading/thread.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Thread interface.
@ingroup threading
@ingroup thread
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/core/threading/thread.gen_interface>

namespace togo {
namespace thread {

/**
	@addtogroup thread
	@{
*/

// implementation

/// Name.
char const* name(Thread* t);

/// Whether the current thread is the main thread.
bool is_main();

/// Yield to the processor on the current thread.
void yield();

/// Create thread.
///
/// Execution will begin as soon as the system allots time to the
/// thread.
/// An assertion will fail if a thread could not be created.
/// name will be copied and truncated to 32 characters.
/// func takes the specified call_data.
/// allocator will be used to construct and destroy the thread
/// object, so its lifetime must be longer than the thread's.
Thread* create(
	char const* name,
	void* call_data,
	void* (*func)(void* call_data),
	Allocator& allocator = memory::default_allocator()
);

/// Join thread.
///
/// The thread object will be invalid after this call. The return
/// value is the return value of the function passed to
/// thread::create(). This blocks until the thread completes its
/// execution.
void* join(Thread* t);

/** @} */ // end of doc-group thread

} // namespace thread
} // namespace togo
