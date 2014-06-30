#line 2 "togo/condvar.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file condvar.hpp
@brief CondVar interface.
@ingroup threading
@ingroup condvar
*/

#pragma once

#include <togo/config.hpp>
#include <togo/threading_types.hpp>

namespace togo {

/**
	@addtogroup threading
	@{
*/

namespace condvar {

/**
	@addtogroup condvar
	@{
*/

/// Signal a condition variable.
///
/// A single thread waiting on the condition variable will be
/// unsuspended.
void signal(CondVar& cv, MutexLock& l);

/// Wait for a condition variable to be signaled.
///
/// The mutex lock will be atomically released before suspending
/// thread execution and acquired again before returning.
void wait(CondVar& cv, MutexLock& l);

/** @} */ // end of doc-group condvar

} // namespace condvar

/// Construct condition variable.
inline CondVar::CondVar()
	: _impl(*this)
{}

/** @} */ // end of doc-group threading

} // namespace togo
