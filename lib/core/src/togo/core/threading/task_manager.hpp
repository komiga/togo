#line 2 "togo/core/threading/task_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief TaskManager interface.
@ingroup threading
@ingroup task_manager
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/core/threading/task_manager.gen_interface>

namespace togo {
namespace task_manager {

/**
	@addtogroup task_manager
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

/** @} */ // end of doc-group task_manager

} // namespace task_manager
} // namespace togo
