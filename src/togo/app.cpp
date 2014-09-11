#line 2 "togo/app.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/app_type.hpp>
#include <togo/log.hpp>
#include <togo/system.hpp>
#include <togo/memory.hpp>
#include <togo/gfx/init.hpp>
#include <togo/gfx/display.hpp>
#include <togo/input_buffer.hpp>

namespace togo {

AppBase::~AppBase() {}

AppBase::AppBase(
	init_func_type func_init,
	shutdown_func_type func_shutdown,
	update_func_type func_update,
	render_func_type func_render,
	unsigned num_args,
	char const* args[],
	float update_freq
)
	: _func_init(func_init)
	, _func_shutdown(func_shutdown)
	, _func_update(func_update)
	, _func_render(func_render)
	, _num_args(num_args)
	, _args(args)
	, _display(nullptr)
	, _input_buffer(memory::default_allocator())
	, _task_manager(
		system::num_cores() - 1u,
		memory::default_allocator()
	)
	, _update_freq(update_freq)
	, _quit(false)
{}

namespace app {

void core_quit(AppBase& app_base);
void core_run(AppBase& app_base);

static void core_init(AppBase& app_base);
static void core_shutdown(AppBase& app_base);
static void core_update(AppBase& app_base, float dt);
static void core_render(AppBase& app_base);

static void core_init(AppBase& app_base) {
	TOGO_LOG("App: initializing\n");
	gfx::init(3, 3);
	gfx::Config config{};
	config.color_bits = {8, 8, 8, 0};
	config.depth_bits = 16;
	config.stencil_bits = 0;
	config.msaa_num_buffers = 0;
	config.msaa_num_samples = 0;
	config.flags = gfx::ConfigFlags::double_buffered;
	app_base._display = gfx::display::create(
		"togo display",
		1024, 768,
		gfx::DisplayFlags::borderless |
		gfx::DisplayFlags::resizable,
		config
	);
	input_buffer::add_display(app_base._input_buffer, app_base._display);
	gfx::display::set_mouse_lock(app_base._display, false);
	app_base._quit = false;
	app_base._func_init(app_base);
}

static void core_shutdown(AppBase& app_base) {
	TOGO_LOG("App: shutting down\n");
	app_base._func_shutdown(app_base);
	input_buffer::remove_display(app_base._input_buffer, app_base._display);
	gfx::display::destroy(app_base._display);
	gfx::shutdown();
}

static void core_update(AppBase& app_base, float dt) {
	InputEventType event_type{};
	InputEvent const* event = nullptr;
	while (input_buffer::poll(app_base._input_buffer, event_type, event)) {
		if (
			event_type == InputEventType::display_close_request &&
			event->display == app_base._display
		) {
			app::core_quit(app_base);
		}
	}
	app_base._func_update(app_base, dt);
}

static void core_render(AppBase& app_base) {
	app_base._func_render(app_base);
	gfx::display::swap_buffers(app_base._display);
}

void core_run(AppBase& app_base) {
	app::core_init(app_base);
	float const update_freq = app_base._update_freq;
	float time_prev = system::time_monotonic();
	float time_next;
	float time_delta;
	float time_accum = update_freq;
	bool do_render = false;
	while (!app_base._quit) {
		time_next = system::time_monotonic();
		time_delta = time_next - time_prev;
		time_prev = time_next;
		time_accum += time_delta;
		do_render = time_accum >= update_freq;
		while (time_accum >= update_freq) {
			time_accum -= update_freq;
			app::core_update(app_base, update_freq);
		}
		if (do_render) {
			app::core_render(app_base);
		}
		system::sleep_ms(1);
	}
	app::core_shutdown(app_base);
}

void core_quit(AppBase& app_base) {
	TOGO_LOG("App: quitting\n");
	app_base._quit = true;
}

} // namespace app
} // namespace togo
