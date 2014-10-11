
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/memory.hpp>
#include <togo/priority_queue.hpp>
#include <togo/mutex.hpp>
#include <togo/thread.hpp>
#include <togo/task_manager.hpp>

#include "../common/helpers.hpp"

using namespace togo;

static Mutex counter_mutex{};
static unsigned counter = 0;

struct WorkNum {
	unsigned value;
};

void task_func(TaskID const task_id, void* const data) {
	WorkNum const* const wnum = static_cast<WorkNum*>(data);
	TOGO_LOGF(
		"task: %08x on %-32s: wnum: %u\n",
		task_id._value,
		thread::name(),
		wnum->value
	);
	MutexLock l{counter_mutex};
	++counter;
}

TaskWork task_work_num(WorkNum& wnum) {
	return {&wnum, task_func};
}

signed main() {
	WorkNum wnum_list[]{
		{10}, {9}, {8}, {7}, {6},
		{ 5}, {4}, {3}, {2}, {1}
	};
	memory_init();

	{
		TaskManager tm{5, memory::default_allocator()};
		unsigned count = 10;
		while (count--) {
			TaskID const finish_id = task_manager::add_hold_empty(tm);
			TOGO_LOGF("finish_id: %08x\n", finish_id._value);
			for (auto& wnum : wnum_list) {
				TaskID const work_id = task_manager::add(
					tm, task_work_num(wnum), static_cast<u16>(wnum.value)
				);
				task_manager::set_parent(tm, work_id, finish_id);
			}
			task_manager::end_hold(tm, finish_id);
			task_manager::wait(tm, finish_id);
			TOGO_LOG("finished\n\n");
			TOGO_ASSERTE(counter == 10);
			counter = 0;
		}
	}
	return 0;
}
