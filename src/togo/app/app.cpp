#line 2 "togo/app/app.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/system/system.hpp>
#include <togo/entity/entity_manager.hpp>
#include <togo/world/world_manager.hpp>
#include <togo/gfx/gfx.hpp>
#include <togo/gfx/display.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/input/input_buffer.hpp>
#include <togo/resource/resource_handler.hpp>
#include <togo/resource/resource_manager.hpp>
#include <togo/app/types.hpp>
#include <togo/app/app.hpp>

namespace togo {

namespace app {

app::Globals _app_globals{
	nullptr,
	nullptr
};

void init_with(Allocator& allocator, AppBase* app_base);
void actual_init(AppBase& app_base);
void update(AppBase& app_base, float dt);
void render(AppBase& app_base);

} // namespace app

AppBase::~AppBase() {}

AppBase::AppBase(
	destruct_func_type& func_destruct,
	init_func_type& func_init,
	shutdown_func_type& func_shutdown,
	update_func_type& func_update,
	render_func_type& func_render,
	unsigned num_args,
	char const* const args[],
	StringRef const base_path,
	float update_freq
)
	: _func_destruct(func_destruct)
	, _func_init(func_init)
	, _func_shutdown(func_shutdown)
	, _func_update(func_update)
	, _func_render(func_render)
	, _num_args(num_args)
	, _args(args)
	, _task_manager(
		system::num_cores() - 1u,
		memory::default_allocator()
	)
	, _resource_manager(
		base_path,
		memory::default_allocator()
	)
	, _entity_manager(memory::default_allocator())
	, _world_manager(memory::default_allocator())
	, _display(nullptr)
	, _input_buffer(memory::default_allocator())
	, _renderer(nullptr)
	, _update_freq(update_freq)
	, _quit(false)
{}

void app::init_with(Allocator& allocator, AppBase* app_base) {
	TOGO_ASSERT(
		!_app_globals.instance,
		"application has already been initialized"
	);
	_app_globals.allocator = &allocator;
	_app_globals.instance = app_base;
	app::actual_init(*app_base);
}

void app::actual_init(AppBase& app_base) {
	TOGO_LOG("App: initializing\n");

	// Initialize graphics
	gfx::init(3, 3);
	gfx::DisplayConfig config{};
	config.color_bits = {8, 8, 8, 0};
	config.depth_bits = 16;
	config.stencil_bits = 0;
	config.msaa_num_buffers = 0;
	config.msaa_num_samples = 0;
	config.flags = gfx::DisplayConfigFlags::double_buffered;
	app_base._display = gfx::display::create(
		"togo display",
		1024, 768,
		gfx::DisplayFlags::borderless |
		gfx::DisplayFlags::resizable,
		config,
		nullptr,
		memory::default_allocator()
	);
	input_buffer::add_display(app_base._input_buffer, app_base._display);
	gfx::display::set_mouse_lock(app_base._display, false);
	app_base._renderer = gfx::renderer::create(
		memory::default_allocator()
	);

	// Register resource handlers
	resource_handler::register_test(app_base._resource_manager);
	resource_handler::register_shader_prelude(app_base._resource_manager, app_base._renderer);
	resource_handler::register_shader(app_base._resource_manager, app_base._renderer);
	resource_handler::register_render_config(app_base._resource_manager, app_base._renderer);

	// Register components
	//components::register_transform3d(app_base._world_manager);
	//components::register_mesh(app_base._world_manager);
	//components::register_camera(app_base._world_manager);

	app_base._quit = false;
	app_base._func_init(app_base);
}

void app::shutdown() {
	auto& app_base = app::instance();
	TOGO_LOG("App: shutting down\n");
	app_base._func_shutdown(app_base);

	world_manager::shutdown(app_base._world_manager);
	entity_manager::shutdown(app_base._entity_manager);

	resource_manager::clear_resources(app_base._resource_manager);
	gfx::renderer::destroy(app_base._renderer);
	app_base._renderer = nullptr;
	input_buffer::remove_display(app_base._input_buffer, app_base._display);
	gfx::display::destroy(app_base._display);
	app_base._display = nullptr;
	gfx::shutdown();
	resource_manager::clear_packages(app_base._resource_manager);
	resource_manager::clear_handlers(app_base._resource_manager);

	app_base._func_destruct(app_base);
	TOGO_DESTROY(*_app_globals.allocator, &app_base);
	_app_globals.allocator = nullptr;
	_app_globals.instance = nullptr;
}

void app::update(AppBase& app_base, float dt) {
	InputEventType event_type{};
	InputEvent const* event = nullptr;
	input_buffer::update(app_base._input_buffer);
	while (input_buffer::poll(app_base._input_buffer, event_type, event)) {
		if (event->display != app_base._display) {
			continue;
		}
		switch (event_type) {
		case InputEventType::display_close_request:
			app::quit();
			break;

		case InputEventType::display_resize:
			gfx::renderer::set_viewport_size(
				app_base._renderer,
				event->display_resize.new_width,
				event->display_resize.new_height
			);
			break;

		default:
			break;
		}
	}
	app_base._func_update(app_base, dt);
}

void app::render(AppBase& app_base) {
	app_base._func_render(app_base);
	gfx::display::swap_buffers(app_base._display);
}

void app::run() {
	auto& app_base = app::instance();
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
			app::update(app_base, update_freq);
		}
		if (do_render) {
			app::render(app_base);
		}
		system::sleep_ms(1);
	}
}

void app::quit() {
	auto& app_base = app::instance();
	TOGO_LOG("App: quitting\n");
	app_base._quit = true;
}

} // namespace togo
