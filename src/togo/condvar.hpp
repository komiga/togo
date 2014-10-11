#line 2 "togo/condvar.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief CondVar interface.
@ingroup threading
@ingroup condvar
*/

#pragma once

#include <togo/config.hpp>
#include <togo/threading_types.hpp>

namespace togo {

namespace condvar {

/**
	@addtogroup condvar
	@{
*/

/// Signal a single thread waiting on the condition variable.
///
/// This makes no actual constraint, but the mutex must already be
/// locked.
void signal(CondVar& cv, Mutex& m);

/// Signal a single thread waiting on the condition variable.
inline void signal(CondVar& cv, MutexLock& l) {
	signal(cv, l._mutex);
}

/// Signal all threads waiting on the condition variable.
///
/// This makes no actual constraint, but the mutex must already be
/// locked.
void signal_all(CondVar& cv, Mutex& m);

/// Signal all threads waiting on the condition variable.
inline void signal_all(CondVar& cv, MutexLock& l) {
	signal_all(cv, l._mutex);
}

/// Wait for a condition variable to be signaled.
///
/// This makes no actual constraint, but the mutex must already be
/// locked. The mutex will be atomically unlocked before suspending
/// thread execution and locked again before returning (when
/// signaled).
void wait(CondVar& cv, Mutex& m);

/// Wait for a condition variable to be signaled.
///
/// The mutex lock will be atomically released before suspending
/// thread execution and acquired again before returning (when
/// signaled).
inline void wait(CondVar& cv, MutexLock& l) {
	wait(cv, l._mutex);
}

/** @} */ // end of doc-group condvar

} // namespace condvar

/// Construct condition variable.
inline CondVar::CondVar()
	: _impl(*this)
{}

} // namespace togo
