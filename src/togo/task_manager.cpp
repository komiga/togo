#line 2 "togo/task_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/array.hpp>
#include <togo/priority_queue.hpp>
#include <togo/thread.hpp>
#include <togo/mutex.hpp>
#include <togo/condvar.hpp>
#include <togo/task_manager.hpp>

#include <cstring>
#include <cstdio>

#undef TOGO_TEST_LOG_ENABLE
#if defined(TOGO_TEST_TASK_MANAGER)
	#define TOGO_TEST_LOG_ENABLE 
#endif
#include <togo/log_test.hpp>

namespace togo {

namespace {

static constexpr TaskID const ID_NULL{0};

inline bool operator==(TaskID const& x, TaskID const& y) {
	return x._value == y._value;
}

inline bool operator!=(TaskID const& x, TaskID const& y) {
	return x._value != y._value;
}

inline unsigned task_sort_key(Task const& task) {
	// Sort by num_incomplete, then by priority
	return (~unsigned{task.num_incomplete} << 16 & 0xFFFF0000) | unsigned{task.priority};
}

bool task_less(Task* const& x, Task* const& y) {
	return task_sort_key(*x) < task_sort_key(*y);
}

enum : unsigned {
	// 128 tasks
	ID_SHIFT = 7,
	NUM_TASKS = 1 << ID_SHIFT,
	ID_ADD = 1 << ID_SHIFT,
	INDEX_MASK = NUM_TASKS - 1,

	FLAG_SHUTDOWN = 1 << 0,
};

inline Task& get_task(TaskManager& tm, TaskID const id) {
	return tm._tasks[id._value & INDEX_MASK].task;
}

inline bool is_complete(Task const& task, TaskID const expected_id) {
	return task.id != expected_id || task.num_incomplete == 0;
}

inline bool is_ready(Task const& task) {
	return task.num_incomplete <= 1;
}

inline void free_slot(TaskManager& tm, Task& task) {
	TaskSlot& slot = *reinterpret_cast<TaskSlot*>(&task);
	unsigned index = slot.id._value & INDEX_MASK;
	slot.hole.next = nullptr;
	while (index--) {
		TaskSlot& slot_before = tm._tasks[index];
		if (slot_before.id == ID_NULL) {
			slot.hole.next = slot_before.hole.next;
			slot_before.hole.next = &slot;
			break;
		}
	}
	if (!slot.hole.next) {
		// No hole between slot and head
		slot.hole.next = tm._first_hole;
		tm._first_hole = &slot;
	}
	slot.id = ID_NULL;
}

inline Task& add_task(
	TaskManager& tm,
	TaskWork const& work,
	u16 const priority
) {
	TOGO_ASSERT(tm._num_tasks != NUM_TASKS, "cannot add task to full task manager");
	TOGO_DEBUG_ASSERTE(tm._first_hole != nullptr);
	TaskSlot& slot = *tm._first_hole;
	tm._first_hole = slot.hole.next;
	std::memset(&slot.task, 0, sizeof(Task));
	slot.task.id._value = tm._id_gen | (&slot - tm._tasks);
	slot.task.work = work;
	slot.task.priority = priority;
	slot.task.num_incomplete = 1;
	tm._id_gen = max(tm._id_gen + ID_ADD, u32{ID_ADD});
	return slot.task;
}

inline void queue_task(TaskManager& tm, Task& task) {
	/*TOGO_TEST_LOG_DEBUGF(
		"queue_task: push: %04u %03u @ %04hu / %04hu\n",
		task.id._value >> ID_SHIFT,
		task.id._value & INDEX_MASK,
		task.num_incomplete,
		task.priority
	);*/
	priority_queue::push(tm._queue, &task);
	/*#if defined(TOGO_TEST_TASK_MANAGER)
		Task* const front = priority_queue::front(tm._queue);
		TOGO_TEST_LOG_DEBUGF(
			"queue_task: front: %04u %03u @ %04hu / %04hu\n",
			front->id._value >> ID_SHIFT,
			front->id._value & INDEX_MASK,
			front->num_incomplete,
			front->priority
		);
	#endif*/
	condvar::signal(tm._work_signal, tm._mutex);
}

inline void complete_task(TaskManager& tm, Task& task) {
	// TODO: Reorder each parent in the priority queue
	TOGO_TEST_LOG_DEBUGF(
		"complete_task  : %-32s: %04u %03u @ %04hu / %04hu\n",
		thread::name(),
		task.id._value >> ID_SHIFT,
		task.id._value & INDEX_MASK,
		task.num_incomplete,
		task.priority
	);
	task.num_incomplete = 0;
	TaskID parent_id = task.parent;
	while (parent_id != ID_NULL) {
		Task& parent = get_task(tm, parent_id);
		--parent.num_incomplete;
		parent_id = parent.num_incomplete ? parent.parent : ID_NULL;
	}
	free_slot(tm, task);
	condvar::signal_all(tm._work_signal, tm._mutex);
}

void execute_pending(TaskManager& tm, TaskID const wait_id) {
	Task* task = nullptr;
	Task const* wait_task = nullptr;
	if (wait_id != ID_NULL) {
		wait_task = &get_task(tm, wait_id);
	}
	MutexLock lock{tm._mutex};
	while (true) {
		if (task) {
			complete_task(tm, *task);
			task = nullptr;
		}
		if (wait_task && is_complete(*wait_task, wait_id)) {
			return;
		} else if (tm._flags & FLAG_SHUTDOWN) {
			return;
		} else if (
			priority_queue::any(tm._queue) &&
			is_ready(*priority_queue::front(tm._queue))
		) {
			task = priority_queue::front(tm._queue);
			priority_queue::pop(tm._queue);
			TOGO_TEST_LOG_DEBUGF(
				"execute_pending: %-32s: %04u %03u @ %04hu / %04hu [take]\n",
				thread::name(),
				task->id._value >> ID_SHIFT,
				task->id._value & INDEX_MASK,
				task->num_incomplete,
				task->priority
			);
			// If !task->work.func, the task is empty
			if (task->work.func) {
				// task->id and task->work should not be modified
				// after adding and the task should not be destroyed
				// by any other function, so this is free of race
				// conditions.
				mutex::unlock(tm._mutex);
				task->work.func(task->id, task->work.data);
				mutex::lock(tm._mutex);
			}
			continue;
		}
		condvar::wait(tm._work_signal, lock);
	}
}

void* worker_func(void* const tm_void) {
	TaskManager& tm = *static_cast<TaskManager*>(tm_void);
	TOGO_TEST_LOG_DEBUGF("worker_func: start: %s\n", thread::name());
	execute_pending(tm, ID_NULL);
	TOGO_TEST_LOG_DEBUGF("worker_func: shutdown: %s\n", thread::name());
	return nullptr;
}

} // anonymous namespace

static_assert(
	NUM_TASKS == array_extent(&TaskManager::_tasks),
	"ID_SHIFT does not fit with the size of TaskManager::_tasks"
);

// class TaskManager implementation

TaskManager::~TaskManager() {
	{
		MutexLock lock{_mutex};
		_flags |= FLAG_SHUTDOWN;
		condvar::signal_all(_work_signal, lock);
	}
	for (Thread* const worker_thread : _workers) {
		thread::join(worker_thread);
	}
}

TaskManager::TaskManager(unsigned num_workers, Allocator& allocator)
	: _tasks()
	, _queue(task_less, allocator)
	, _workers(allocator)
	, _mutex(MutexType::normal)
	, _work_signal()
	, _first_hole(_tasks)
	, _num_tasks(0)
	, _flags(0)
	, _id_gen(ID_ADD)
{
	TaskSlot* const last = _tasks + (NUM_TASKS - 1);
	for (TaskSlot* slot = _tasks; slot != last; ++slot) {
		slot->hole.next = slot + 1;
	}
	last->hole.next = nullptr;
	priority_queue::reserve(_queue, NUM_TASKS);
	if (num_workers) {
		array::reserve(_workers, num_workers);
		char name[32];
		while (num_workers--) {
			std::snprintf(name, sizeof(name), "tm-%8p-worker-%u", this, num_workers);
			array::push_back(_workers, thread::create(name, this, worker_func, allocator));
		}
	}
}

// interface task_manager implementation

TaskID task_manager::add(
	TaskManager& tm,
	TaskWork const& work,
	u16 const priority
) {
	MutexLock lock{tm._mutex};
	Task& task = add_task(tm, work, priority);
	queue_task(tm, task);
	return task.id;
}

TaskID task_manager::add_hold(
	TaskManager& tm,
	TaskWork const& work,
	u16 const priority
) {
	MutexLock lock{tm._mutex};
	return add_task(tm, work, priority).id;
}

void task_manager::set_parent(
	TaskManager& tm,
	TaskID const child_id,
	TaskID const parent_id
) {
	TOGO_ASSERT(child_id != parent_id, "cannot make task a child of itself");
	MutexLock lock{tm._mutex};
	Task& child = get_task(tm, child_id);
	if (child.id != child_id) {
		TOGO_TEST_LOG_DEBUGF(
			"set_parent: child does not exist or has completed: %04u %03u\n",
			child_id._value >> ID_SHIFT,
			child_id._value & INDEX_MASK
		);
		return;
	}
	TOGO_ASSERT(child.parent == ID_NULL, "child task already has a parent");
	TOGO_ASSERT(parent_id != ID_NULL, "parent ID is invalid");
	Task& parent = get_task(tm, parent_id);
	TOGO_ASSERT(parent.id == parent_id, "parent task does not exist");
	++parent.num_incomplete;
	child.parent = parent_id;
}

void task_manager::end_hold(TaskManager& tm, TaskID const id) {
	MutexLock lock{tm._mutex};
	Task& task = get_task(tm, id);
	TOGO_ASSERT(task.id == id, "id is not valid");
	queue_task(tm, task);
}

void task_manager::wait(TaskManager& tm, TaskID const id) {
	TOGO_DEBUG_ASSERT(id != ID_NULL, "attempted to wait on null ID");
	execute_pending(tm, id);
}

} // namespace togo
