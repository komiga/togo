
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/thread.hpp>
#include <togo/mutex.hpp>
#include <togo/condvar.hpp>

#include "../common/helpers.hpp"

#include <cstring>
#include <cstdio>

using namespace togo;

static Mutex hive_mutex{};
static CondVar drone_cv{};
static CondVar queen_cv{};
static unsigned signal_value = 0;

struct Drone {
	Thread* thread;
};

void* drone_func(void*) {
	{
		MutexLock lock{hive_mutex};
		while (signal_value == 0) {
			condvar::wait(drone_cv, lock);
		}
		TOGO_LOGF("%s: signaled with %u\n", thread::name(), signal_value);
		signal_value = 0;
		condvar::signal(queen_cv, lock);
	}
	return nullptr;
}

void* queen_func(void*) {
	static constexpr unsigned const DRONE_COUNT = 20;
	Drone drones[DRONE_COUNT];

	char name[10];
	unsigned index = 1;
	for (auto& drone : drones) {
		std::snprintf(name, sizeof(name), "drone-%u", index);
		drone.thread = thread::create(name, nullptr, drone_func);
		++index;
	}

	for (unsigned value = 1; value <= DRONE_COUNT; ++value) {
		{
			MutexLock lock{hive_mutex};
			signal_value = value;
			condvar::signal(drone_cv, lock);
			while (signal_value != 0) {
				condvar::wait(queen_cv, lock);
			}
			TOGO_LOGF("%s: received return signal\n", thread::name());
		}
	}
	for (auto const& drone : drones) {
		TOGO_LOGF("joining %s\n", thread::name(drone.thread));
		thread::join(drone.thread);
	}
	return nullptr;
}

signed
main() {
	core_init();

	Thread* queen = thread::create("queen", nullptr, queen_func);
	thread::join(queen);
	return 0;
}
