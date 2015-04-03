
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/game/input/input.hpp>
#include <togo/game/app/app.hpp>

#include <togo-test/helpers.hpp>

using namespace togo;

struct TestAppData {
	unsigned x{42};

	~TestAppData() {
		TOGO_LOG("~TestAppData()\n");
	}
};

using TestApp = App<TestAppData>;
using TestAppModel = AppModel<TestAppData>;

template<>
void TestAppModel::init(TestApp& app) {
	TOGO_LOG("init()\n");
	TOGO_ASSERTE(app.data.x == 42);
}

template<>
void TestAppModel::shutdown(TestApp& app) {
	TOGO_LOG("shutdown()\n");
	TOGO_ASSERTE(app.data.x == 42);
}

template<>
void TestAppModel::update(TestApp& app, float /*dt*/) {
	//TOGO_LOG("update()\n");
	if (input::key_released(app.display, KeyCode::escape)) {
		app::quit();
	}
}

template<>
void TestAppModel::render(TestApp& /*app*/) {
	//TOGO_LOG("render()\n");
}

signed main(signed argc, char* argv[]) {
	memory_init();

	app::init<TestAppData>(
		memory::default_allocator(),
		array_ref(unsigned_cast(max(0, argc - 1)), argv),
		"data/project/package/",
		1.0f / 30.0f
	);
	app::run();
	app::shutdown();
	return 0;
}
