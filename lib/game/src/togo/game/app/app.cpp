#line 2 "togo/game/app/app.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/threading/task_manager.hpp>
#include <togo/game/entity/entity_manager.hpp>
#include <togo/game/world/world_manager.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/display.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/input/input_buffer.hpp>
#include <togo/game/resource/resource_handler.hpp>
#include <togo/game/resource/resource_manager.hpp>
#include <togo/game/app/types.hpp>
#include <togo/game/app/app.hpp>

namespace togo {

namespace app {

app::Globals _globals{
	nullptr,
	nullptr
};

} // namespace app

AppBase::~AppBase() {}

AppBase::AppBase(
	destruct_func_type& func_destruct,
	init_func_type& func_init,
	shutdown_func_type& func_shutdown,
	update_func_type& func_update,
	render_func_type& func_render,
	ArrayRef<char const* const> args,
	StringRef const base_path,
	float update_freq
)
	: _func_destruct(func_destruct)
	, _func_init(func_init)
	, _func_shutdown(func_shutdown)
	, _func_update(func_update)
	, _func_render(func_render)
	, args(args)
	, task_manager(
		system::num_cores() - 1u,
		memory::default_allocator()
	)
	, resource_manager(
		base_path,
		memory::default_allocator()
	)
	, entity_manager(memory::default_allocator())
	, world_manager(memory::default_allocator())
	, display(nullptr)
	, input_buffer(memory::default_allocator())
	, renderer(nullptr)
	, _update_freq(update_freq)
	, _quit(false)
{}

IGEN_PRIVATE
void app::init_with(Allocator& allocator, AppBase* app) {
	TOGO_ASSERT(
		!app::_globals.instance,
		"application has already been initialized"
	);
	app::_globals.allocator = &allocator;
	app::_globals.instance = app;
	app::actual_init(*app);
}

IGEN_PRIVATE
void app::actual_init(AppBase& app) {
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
	app.display = gfx::display::create(
		"togo display",
		1024, 768,
		gfx::DisplayFlags::borderless |
		gfx::DisplayFlags::resizable,
		config,
		nullptr,
		memory::default_allocator()
	);
	input_buffer::add_display(app.input_buffer, app.display);
	gfx::display::set_mouse_lock(app.display, false);
	app.renderer = gfx::renderer::create(
		memory::default_allocator()
	);

	// Register resource handlers
	resource_handler::register_test(app.resource_manager);
	resource_handler::register_shader_prelude(app.resource_manager, app.renderer);
	resource_handler::register_shader(app.resource_manager, app.renderer);
	resource_handler::register_render_config(app.resource_manager, app.renderer);

	// Register components
	//components::register_transform3d(app.world_manager);
	//components::register_mesh(app.world_manager);
	//components::register_camera(app.world_manager);

	app._quit = false;
	app._func_init(app);
}

/// Shutdown application.
///
/// An assertion will fail if the application has not been created.
void app::shutdown() {
	auto& app = app::instance();
	TOGO_LOG("App: shutting down\n");
	app._func_shutdown(app);

	world_manager::shutdown(app.world_manager);
	entity_manager::shutdown(app.entity_manager);

	resource_manager::clear_resources(app.resource_manager);
	gfx::renderer::destroy(app.renderer);
	app.renderer = nullptr;
	input_buffer::remove_display(app.input_buffer, app.display);
	gfx::display::destroy(app.display);
	app.display = nullptr;
	gfx::shutdown();
	resource_manager::clear_packages(app.resource_manager);
	resource_manager::clear_handlers(app.resource_manager);

	app._func_destruct(app);
	TOGO_DESTROY(*app::_globals.allocator, &app);
	app::_globals.allocator = nullptr;
	app::_globals.instance = nullptr;
}

IGEN_PRIVATE
void app::update(AppBase& app, float dt) {
	InputEventType event_type{};
	InputEvent const* event = nullptr;
	input_buffer::update(app.input_buffer);
	while (input_buffer::poll(app.input_buffer, event_type, event)) {
		if (event->display != app.display) {
			continue;
		}
		switch (event_type) {
		case InputEventType::display_close_request:
			app::quit();
			break;

		case InputEventType::display_resize:
			gfx::renderer::set_viewport_size(
				app.renderer,
				event->display_resize.new_width,
				event->display_resize.new_height
			);
			break;

		default:
			break;
		}
	}
	app._func_update(app, dt);
}

IGEN_PRIVATE
void app::render(AppBase& app) {
	gfx::display::unbind_context();
	TaskID const work_task_id = gfx::renderer::begin_frame(
		app.renderer, app.task_manager, app.display
	);
	app._func_render(app);
	gfx::renderer::end_frame(app.renderer);
	task_manager::wait(app.task_manager, work_task_id);
	gfx::display::bind_context(app.display);
}

/// Run the application.
///
/// This will run the main loop until a quit is signaled.
void app::run() {
	auto& app = app::instance();
	float const update_freq = app._update_freq;
	float time_prev = system::time_monotonic();
	float time_next;
	float time_delta;
	float time_accum = update_freq;
	bool do_render = false;
	while (!app._quit) {
		time_next = system::time_monotonic();
		time_delta = time_next - time_prev;
		time_prev = time_next;
		time_accum += time_delta;
		do_render = time_accum >= update_freq;
		while (time_accum >= update_freq) {
			time_accum -= update_freq;
			app::update(app, update_freq);
		}
		if (do_render) {
			app::render(app);
		}
		system::sleep_ms(1);
	}
}

/// Signal the application to quit.
///
/// This has no effect if the application is not running
/// (see app::run()).
void app::quit() {
	auto& app = app::instance();
	TOGO_LOG("App: quit requested\n");
	app._quit = true;
}

/// Render a world through a camera and viewport.
void app::render_world(
	WorldID const world_id,
	EntityID const camera_id,
	gfx::ViewportNameHash const viewport_name_hash
) {
	auto& app = app::instance();
	gfx::renderer::push_work(
		app.renderer,
		gfx::CmdRenderWorld{world_id, camera_id, viewport_name_hash}
	);
}

} // namespace togo
