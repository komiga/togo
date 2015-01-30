
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/math/math.hpp>
#include <togo/log/log.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource_handler.hpp>
#include <togo/resource/resource_manager.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/gfx.hpp>
#include <togo/gfx/display.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/gfx/renderer/private.hpp>
#include <togo/gfx/renderer/opengl.hpp>
#include <togo/input/input.hpp>
#include <togo/app/app.hpp>

#include "../common/helpers.hpp"

#include <cmath>
#include <initializer_list>

using namespace togo;
using namespace togo::gfx::hash_literals;

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
	f32 rg;
	f32 gb;
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
	resource_manager::add_package(app._resource_manager, "test_data");
	gfx::display::set_swap_mode(app._display, gfx::DisplaySwapMode::wait_refresh);

	app._data.osc_paused = false;

	// Setup mesh data
	app._data.triangle_buffer = gfx::renderer::create_buffer(
		app._renderer, sizeof(triangle_vertices), triangle_vertices
	);
	app._data.triangle_binding = gfx::renderer::create_buffer_binding(
		app._renderer, array_extent(triangle_vertices), 0, {},
		{{app._data.triangle_buffer, &triangle_vformat, 0}}
	);

	// Setup parameter block buffer
	// TODO: Need an alignment function for the size based on the
	// number of param blocks that will be used in the buffer.
	// Alignment of 256 bytes is common ;_;
	app._data.pb_buffer = gfx::renderer::create_buffer(
		app._renderer,
		sizeof(ColorFactors) + 256 + sizeof(Oscillator),
		nullptr, gfx::BufferDataBinding::dynamic
	);

	// Setup ColorFactors parameter block
	app._data.p_color_factors.rg = 1.0f;
	app._data.p_color_factors.gb = 1.0f;
	app._data.p_color_factors_binding = gfx::renderer::make_param_block_binding(
		app._renderer,
		app._data.pb_buffer, 0, sizeof(ColorFactors)
	);
	gfx::renderer::set_fixed_param_block(
		app._renderer,
		0, "ColorFactors"_param_block_name,
		app._data.p_color_factors_binding
	);

	// Setup Oscillator parameter block
	app._data.p_osc.time = MC_PI;
	app._data.p_osc_binding = gfx::renderer::make_param_block_binding(
		app._renderer,
		app._data.pb_buffer, sizeof(ColorFactors), sizeof(Oscillator)
	);

	// Load shader
	app._data.shader = resource::load_shader(
		app._resource_manager,
		"test/gfx/simple-oscillate"_resource_name
	);
}

template<>
void TestAppModel::shutdown(TestApp& app) {
	gfx::renderer::destroy_buffer(app._renderer, app._data.pb_buffer);
	gfx::renderer::destroy_buffer_binding(app._renderer, app._data.triangle_binding);
	gfx::renderer::destroy_buffer(app._renderer, app._data.triangle_buffer);
}

f32 usin(f32 x) {
	return std::fabs(std::sin(x));
}

template<>
void TestAppModel::update(TestApp& app, float dt) {
	if (input::key_released(app._display, KeyCode::space)) {
		app._data.osc_paused = !app._data.osc_paused;
	}
	if (input::key_released(app._display, KeyCode::escape)) {
		app::quit(app);
	}

	// Update p_osc
	constexpr f32 const m = MC_TAU * 100.0f;
	if (!app._data.osc_paused) {
		app._data.p_osc.time = std::fmod(app._data.p_osc.time + dt, m);
	}

	// Update p_color_factors
	Vec2 const mp = input::mouse_position(app._display);
	app._data.p_color_factors.rg = mp.x / static_cast<f32>(gfx::display::width(app._display));
	app._data.p_color_factors.gb = mp.y / static_cast<f32>(gfx::display::height(app._display));
}

template<>
void TestAppModel::render(TestApp& app) {
	gfx::renderer::clear_backbuffer(app._renderer);
	gfx::renderer::map_buffer(
		app._renderer,
		app._data.p_color_factors_binding, &app._data.p_color_factors
	);
	gfx::renderer::map_buffer(
		app._renderer,
		app._data.p_osc_binding, &app._data.p_osc
	);
	gfx::renderer::render_objects(
		app._renderer,
		app._data.shader,
		1, &app._data.p_osc_binding,
		1, &app._data.triangle_binding
	);
}

signed main(signed argc, char* argv[]) {
	memory_init();

	TestApp app{
		unsigned_cast(max(0, argc - 1)),
		argv,
		"data/project/package/",
		1.0f / 30.0f,
		TestAppData{}
	};
	app::run(app);
	return 0;
}
