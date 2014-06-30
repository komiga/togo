#line 2 "togo/threading_types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file threading_types.hpp
@brief Threading types.
@ingroup types
@ingroup threading
*/

#pragma once

#include <togo/config.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/mutex/posix.hpp>
#else
	#error "missing mutex implementation for target platform"
#endif

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/condvar/posix.hpp>
#else
	#error "missing condvar implementation for target platform"
#endif

namespace togo {

/**
	@addtogroup threading
	@{
*/

/**
	@addtogroup thread
	@{
*/

struct Thread;

/** @} */ // end of doc-group thread

/**
	@addtogroup mutex
	@{
*/

/// Mutex type.
enum class MutexType : unsigned {
	/// Normal mutex.
	normal = 1,
	/// Recursive mutex.
	recursive,
};

/**
	Mutex.
*/
struct Mutex {
	MutexType _type;
	MutexImpl _impl;

	~Mutex() = default;
	Mutex(Mutex&&) = default;
	Mutex& operator=(Mutex&&) = default;

	Mutex(Mutex const&) = delete;
	Mutex& operator=(Mutex const&) = delete;

	Mutex(MutexType const type = MutexType::normal);
};

/**
	Automatic mutex lock.
*/
struct MutexLock {
	Mutex& _mutex;

	MutexLock(MutexLock&&) = default;
	MutexLock& operator=(MutexLock&&) = default;

	MutexLock() = delete;
	MutexLock(MutexLock const&) = delete;
	MutexLock& operator=(MutexLock const&) = delete;

	~MutexLock();
	MutexLock(Mutex& m);
};

/** @} */ // end of doc-group mutex

/**
	@addtogroup condvar
	@{
*/

/**
	Condition variable.
*/
struct CondVar {
	CondVarImpl _impl;

	~CondVar() = default;
	CondVar(CondVar&&) = default;
	CondVar& operator=(CondVar&&) = default;

	CondVar(CondVar const&) = delete;
	CondVar& operator=(CondVar const&) = delete;

	CondVar();
};

/** @} */ // end of doc-group condvar

/** @} */ // end of doc-group threading

} // namespace togo
