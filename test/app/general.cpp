
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/input/input.hpp>
#include <togo/app/app.hpp>

#include "../common/helpers.hpp"

using namespace togo;

struct TestAppData {
	unsigned x;
};

template<>
void AppModel<TestAppData>::init(App<TestAppData>& app) {
	TOGO_LOG("init()\n");
	TOGO_ASSERTE(app._data.x == 42);
}

template<>
void AppModel<TestAppData>::shutdown(App<TestAppData>& app) {
	TOGO_LOG("shutdown()\n");
	TOGO_ASSERTE(app._data.x == 42);
}

template<>
void AppModel<TestAppData>::update(App<TestAppData>& app, float /*dt*/) {
	//TOGO_LOG("update()\n");
	if (input::key_released(app._display, KeyCode::escape)) {
		app::quit(app);
	}
}

template<>
void AppModel<TestAppData>::render(App<TestAppData>& /*app*/) {
	//TOGO_LOG("render()\n");
}

signed main(signed argc, char* argv[]) {
	memory_init();

	App<TestAppData> app{
		static_cast<unsigned>(max(0, argc - 1)),
		argv,
		1.0f / 30.0f,
		TestAppData{42}
	};
	app::run(app);
	return 0;
}
