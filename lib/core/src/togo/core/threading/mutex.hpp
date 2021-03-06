#line 2 "togo/core/threading/mutex.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Mutex interface.
@ingroup lib_core_threading
@ingroup lib_core_mutex

@defgroup lib_core_mutex Mutex
@ingroup lib_core_threading
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/threading/types.hpp>

namespace togo {

namespace mutex {

/**
	@addtogroup lib_core_mutex
	@{
*/

/// Attempt to lock a mutex.
///
/// Returns true if the lock attempt succeeded; returns false if the
/// normal mutex was already locked on any thread.
/// With a recursive mutex, this behaves as lock().
bool try_lock(Mutex& m);

/// Lock a mutex directly.
///
/// This will block until the lock can be acquired from another
/// thread. If the mutex is already locked on the current thread, it
/// can fail in debug mode, but may silently fail in non-debug mode.
/// If the mutex is recursive, a locked mutex can only be locked again
/// by the thread on which it was first locked.
void lock(Mutex& m);

/// Unlock a mutex directly.
///
/// This will fail if the mutex is not locked or if the current thread
/// is not the same as the thread that locked the thread.
/// Recursive mutexes require the same number of locks as unlocks.
/// A recursive mutex is only unlocked once each lock operation has
/// been undone.
void unlock(Mutex& m);

/** @} */ // end of doc-group lib_core_mutex

} // namespace mutex

/// Construct typed mutex.
inline Mutex::Mutex(MutexType const type)
	: _type(type)
	, _impl(*this)
{}

/// Destroy lock.
///
/// This will unlock the mutex.
inline MutexLock::~MutexLock() {
	mutex::unlock(_mutex);
}

/// Construct lock.
///
/// This will immediately lock the mutex.
inline MutexLock::MutexLock(Mutex& m)
	: _mutex(m)
{
	mutex::lock(_mutex);
}

} // namespace togo
