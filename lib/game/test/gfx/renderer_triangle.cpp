
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/vector/2.hpp>
#include <togo/core/math/vector/3_type.hpp>
#include <togo/core/log/log.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/input/input.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/command.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/gfx/renderer/private.hpp>
#include <togo/game/gfx/renderer/opengl.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource_handler.hpp>
#include <togo/game/resource/resource_manager.hpp>
#include <togo/game/app/app.hpp>

#include <togo/support/test.hpp>

#include <cmath>
#include <initializer_list>

using namespace togo;
using namespace togo::game;
using namespace togo::game::gfx::hash_literals;

struct Vertex {
	Vec2 pos;
	Vec3 color;
};

static gfx::VertexFormat const triangle_vformat{
	{gfx::Primitive::f32, 2, false}, // a_pos
	{gfx::Primitive::f32, 3, false}, // a_color
};

static Vertex const triangle_vertices[]{
	{Vec2{ 0.0f, 1.0f}, Vec3{1.0f,0.0f,0.0f}}, // Vertex 0: top, red
	{Vec2{ 1.0f,-1.0f}, Vec3{0.0f,0.0f,1.0f}}, // Vertex 1: right, blue
	{Vec2{-1.0f,-1.0f}, Vec3{0.0f,1.0f,0.0f}}, // Vertex 2: left, green
};

struct ColorFactors {
	Vec2 rg_gb;
};

struct Oscillator {
	f32 time;
};

struct TestAppData {
	bool osc_paused;
	ColorFactors p_color_factors;
	Oscillator p_osc;

	gfx::BufferID triangle_buffer;
	gfx::BufferBindingID triangle_binding;
	gfx::BufferID pb_buffer;
	gfx::ShaderID shader;

	gfx::ParamBlockBinding p_color_factors_binding;
	gfx::ParamBlockBinding p_osc_binding;
};

using TestApp = App<TestAppData>;
using TestAppModel = AppModel<TestAppData>;

template<>
void TestAppModel::init(TestApp& app) {
	resource_manager::add_package(app.resource_manager, "test_data");
	window::set_swap_mode(app.window, WindowSwapMode::wait_refresh);

	app.data.osc_paused = false;

	// Setup mesh data
	app.data.triangle_buffer = gfx::renderer::create_buffer(
		app.renderer, sizeof(triangle_vertices), triangle_vertices
	);
	app.data.triangle_binding = gfx::renderer::create_buffer_binding(
		app.renderer, array_extent(triangle_vertices), 0, {},
		{{app.data.triangle_buffer, &triangle_vformat, 0}}
	);

	// Setup parameter block buffer
	unsigned const pb_block_size = max(sizeof(ColorFactors), sizeof(Oscillator));
	unsigned const pb_buffer_size = gfx::renderer::param_block_buffer_size(
		app.renderer, 2, pb_block_size
	);
	TOGO_LOGF("pb_block_size = %u, pb_buffer_size = %u\n", pb_block_size, pb_buffer_size);
	app.data.pb_buffer = gfx::renderer::create_buffer(
		app.renderer,
		pb_buffer_size,
		nullptr, gfx::BufferDataBinding::dynamic
	);

	// Setup ColorFactors parameter block
	app.data.p_color_factors.rg_gb = Vec2{1.0f, 1.0f};
	app.data.p_color_factors_binding = gfx::renderer::make_param_block_binding(
		app.renderer,
		app.data.pb_buffer,
		gfx::renderer::param_block_offset(app.renderer, 0, pb_block_size),
		sizeof(ColorFactors)
	);
	gfx::renderer::set_fixed_param_block(
		app.renderer,
		0, "ColorFactors"_param_block_name,
		app.data.p_color_factors_binding
	);

	// Setup Oscillator parameter block
	app.data.p_osc.time = MC_PI;
	app.data.p_osc_binding = gfx::renderer::make_param_block_binding(
		app.renderer,
		app.data.pb_buffer,
		gfx::renderer::param_block_offset(app.renderer, 1, pb_block_size),
		sizeof(Oscillator)
	);

	// Load shader
	app.data.shader = resource::load_shader(
		app.resource_manager,
		"test/gfx/simple-oscillate"_resource_name
	);
}

template<>
void TestAppModel::shutdown(TestApp& app) {
	gfx::renderer::destroy_buffer(app.renderer, app.data.pb_buffer);
	gfx::renderer::destroy_buffer_binding(app.renderer, app.data.triangle_binding);
	gfx::renderer::destroy_buffer(app.renderer, app.data.triangle_buffer);
}

f32 usin(f32 x) {
	return std::fabs(std::sin(x));
}

template<>
void TestAppModel::update(TestApp& app, float dt) {
	if (input::key_released(app.window, KeyCode::space)) {
		app.data.osc_paused = !app.data.osc_paused;
	}
	if (input::key_released(app.window, KeyCode::escape)) {
		app::quit();
	}

	// Update p_osc
	constexpr f32 const m = MC_TAU * 100.0f;
	if (!app.data.osc_paused) {
		app.data.p_osc.time = std::fmod(app.data.p_osc.time + dt, m);
		gfx::renderer::map_buffer(
			app.renderer,
			app.data.p_osc_binding, &app.data.p_osc
		);
	}

	// Update p_color_factors
	Vec2 const mp = input::mouse_position(app.window);
	Vec2 const window_size = window::size(app.window);
	app.data.p_color_factors.rg_gb = mp / window_size;

	gfx::renderer::map_buffer(
		app.renderer,
		app.data.p_color_factors_binding, &app.data.p_color_factors
	);
}

template<>
void TestAppModel::render(TestApp& app) {
	gfx::renderer::push_work(app.renderer, gfx::CmdClearBackbuffer{});
	gfx::renderer::push_work(
		app.renderer,
		gfx::CmdRenderBuffers{
			app.data.shader,
			1, 1,
			&app.data.p_osc_binding,
			&app.data.triangle_binding
		}
	);
}

signed main(signed argc, char* argv[]) {
	memory_init();

	app::init<TestAppData>(
		memory::default_allocator(),
		array_ref(unsigned_cast(argc), argv),
		"data/project/package/",
		1.0f / 30.0f
	);
	app::run();
	app::shutdown();
	return 0;
}
