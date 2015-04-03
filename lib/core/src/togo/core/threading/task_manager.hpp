#line 2 "togo/core/threading/task_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief TaskManager interface.
@ingroup lib_core_threading
@ingroup lib_core_task_manager

@defgroup lib_core_task_manager TaskManager
@ingroup lib_core_threading
@details

TaskManager has a limit of 128 active tasks. An assertion will fail if any
are added when there are no free slots.

If TaskWork has a null func, its task is considered "empty" and no
execution is done for it. Empty tasks can be used to collate sub-tasks and
build task dependencies without incurring extra calls and synchronization
tear down/setup.

If workers are available, they will execute either the next available task
or the next task to be added. As a result, "priority" may not be fully
honored if tasks aren't added in a descending priority order. To ensure the
highest priority tasks are executed first, either setup a hierarchy with
task_manager::add_hold() or add them in descending priority. Furthermore,
lower-priority tasks may be executed before higher-priority tasks due to
preemption.
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/core/threading/task_manager.gen_interface>

namespace togo {
namespace task_manager {

/**
	@addtogroup lib_core_task_manager
	@{
*/

/// Construct an empty TaskWork.
inline TaskWork task_work_empty() {
	return {nullptr, nullptr};
}

/// Add an empty task with a hold on its completion.
inline TaskID add_hold_empty(TaskManager& tm, u16 priority = 0) {
	return add_hold(tm, task_work_empty(), priority);
}

/** @} */ // end of doc-group lib_core_task_manager

} // namespace task_manager
} // namespace togo
