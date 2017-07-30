#line 2 "togo/core/threading/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Threading types.
@ingroup lib_core_types
@ingroup lib_core_threading
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/types.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/core/threading/mutex/posix.hpp>
#else
	#error "missing Mutex implementation for target platform"
#endif

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/core/threading/condvar/posix.hpp>
#else
	#error "missing CondVar implementation for target platform"
#endif

namespace togo {

/**
	@addtogroup lib_core_thread
	@{
*/

struct Thread;

/** @} */ // end of doc-group lib_core_thread

/**
	@addtogroup lib_core_mutex
	@{
*/

/// Mutex type.
enum class MutexType : unsigned {
	/// Normal mutex.
	normal = 1,
	/// Recursive mutex.
	recursive,
};

/// Mutex.
struct Mutex {
	MutexType _type;
	MutexImpl _impl;

	Mutex(Mutex const&) = delete;
	Mutex& operator=(Mutex const&) = delete;

	~Mutex() = default;
	Mutex(Mutex&&) = default;
	Mutex& operator=(Mutex&&) = default;

	Mutex(MutexType const type = MutexType::normal);
};

/// Automatic mutex lock.
struct MutexLock {
	Mutex& _mutex;

	MutexLock() = delete;
	MutexLock(MutexLock const&) = delete;
	MutexLock& operator=(MutexLock const&) = delete;

	MutexLock(MutexLock&&) = default;
	MutexLock& operator=(MutexLock&&) = default;

	~MutexLock();
	MutexLock(Mutex& m);
};

/** @} */ // end of doc-group lib_core_mutex

/**
	@addtogroup lib_core_condvar
	@{
*/

/// Condition variable.
struct CondVar {
	CondVarImpl _impl;

	CondVar(CondVar const&) = delete;
	CondVar& operator=(CondVar const&) = delete;

	~CondVar() = default;
	CondVar(CondVar&&) = default;
	CondVar& operator=(CondVar&&) = default;

	CondVar();
};

/** @} */ // end of doc-group lib_core_condvar

/**
	@addtogroup lib_core_task_manager
	@{
*/

/// Task ID.
struct TaskID {
	u32 _value;
};

/// Task work.
struct TaskWork {
	using func_type = void (
		TaskID task_id,
		void* data
	);

	void* data;
	func_type* func;
};

struct Task {
	TaskID id;
	TaskID parent;
	TaskWork work;
	u16 priority;
	u16 num_incomplete;
};

union TaskSlot;

struct TaskHole {
	TaskID id;
	TaskSlot* next;
};

union TaskSlot {
	TaskID id;
	Task task;
	TaskHole hole;
};

/// Task manager.
struct TaskManager {
	TaskSlot _tasks[128];
	PriorityQueue<Task*> _queue;
	Array<Thread*> _workers;
	Mutex _mutex;
	CondVar _work_signal;

	TaskSlot* _first_hole;
	unsigned _num_tasks;
	unsigned _flags;
	u32 _id_gen;

	TaskManager() = delete;
	TaskManager(TaskManager const&) = delete;
	TaskManager& operator=(TaskManager const&) = delete;

	TaskManager(TaskManager&&) = default;
	TaskManager& operator=(TaskManager&&) = default;

	/// Destroy.
	///
	/// All worker threads will be halted and any waiting threads will
	/// return.
	~TaskManager();

	/// Construct with worker threads and an allocator.
	///
	/// num_workers threads will be created to execute tasks. If
	/// num_workers is 0, tasks can only be executed by a thread that
	/// waits on them.
	///
	/// allocator will be used for all dynamic allocation within the
	/// task manager, including worker threads. The task manager
	/// incurs no extra allocations after the constructor.
	TaskManager(unsigned worker_count, Allocator& allocator);
};

/** @} */ // end of doc-group lib_core_task_manager

} // namespace togo
