
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/math.hpp>
#include <togo/core/log/log.hpp>
#include <togo/game/entity/types.hpp>
#include <togo/game/entity/entity_manager.hpp>
#include <togo/game/world/types.hpp>
#include <togo/game/world/world_manager.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/display.hpp>
#include <togo/game/gfx/command.hpp>
#include <togo/game/gfx/render_node.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/gfx/renderer/private.hpp>
#include <togo/game/gfx/renderer/opengl.hpp>
#include <togo/game/input/input.hpp>
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

struct GenTestProxyData {
	gfx::Renderer* renderer;
	gfx::BufferID buffer;
	gfx::BufferBindingID binding;
	gfx::ShaderID shader;
};

void gen_test_proxy_exec(
	gfx::GeneratorUnit const& unit,
	gfx::RenderNode& node,
	gfx::RenderObject const* /*objects_begin*/,
	gfx::RenderObject const* /*objects_end*/
) {
	auto* data = static_cast<GenTestProxyData*>(unit.data);
	// TODO: Depth-test? Can we get that during cull? Should it be
	// part of RenderObject?
	u64 const key_user = 0;
	gfx::render_node::push(
		node, key_user,
		gfx::CmdRenderBuffers{
			data->shader,
			0, 1,
			nullptr, &data->binding
		}
	);
}

void gen_test_proxy_read(
	gfx::GeneratorDef const& def,
	gfx::Renderer* /*renderer*/,
	BinaryInputSerializer& /*ser*/,
	gfx::GeneratorUnit& unit
) {
	unit.func_exec = gen_test_proxy_exec;
	unit.data = def.data;
}

void gen_test_proxy_destroy(
	gfx::GeneratorDef const& def,
	gfx::Renderer* renderer
) {
	auto* data = static_cast<GenTestProxyData*>(def.data);
	gfx::renderer::destroy_buffer_binding(renderer, data->binding);
	gfx::renderer::destroy_buffer(renderer, data->buffer);
	TOGO_DESTROY(memory::default_allocator(), data);
}

void gen_test_proxy_register() {
	auto& app = app::instance();
	auto* data = TOGO_CONSTRUCT_DEFAULT(
		memory::default_allocator(), GenTestProxyData
	);

	data->renderer = app.renderer;
	data->buffer = gfx::renderer::create_buffer(
		app.renderer, sizeof(triangle_vertices), triangle_vertices
	);
	data->binding = gfx::renderer::create_buffer_binding(
		app.renderer, array_extent(triangle_vertices), 0, {},
		{{data->buffer, &triangle_vformat, 0}}
	);
	data->shader = resource::load_shader(
		app.resource_manager,
		"test/gfx/generic"_resource_name
	);

	gfx::renderer::register_generator_def(
		app.renderer,
		gfx::GeneratorDef{
			"test_proxy"_generator_name,
			data,
			gen_test_proxy_destroy,
			gen_test_proxy_read
		}
	);
}

struct TestAppData {
	WorldID world;
	EntityID camera;
};

using TestApp = App<TestAppData>;
using TestAppModel = AppModel<TestAppData>;

template<>
void TestAppModel::init(TestApp& app) {
	resource_manager::add_package(app.resource_manager, "test_data");
	gfx::display::set_swap_mode(app.display, gfx::DisplaySwapMode::wait_refresh);

	gen_test_proxy_register();
	auto* render_config = resource::load_render_config(
		app.resource_manager,
		"test/gfx/pipeline"_resource_name
	);
	gfx::renderer::configure(app.renderer, *render_config);

	app.data.world = world_manager::create(app.world_manager);
	app.data.camera = entity_manager::create(app.entity_manager);
}

template<>
void TestAppModel::shutdown(TestApp& app) {
	world_manager::destroy(app.world_manager, app.data.world);
	entity_manager::destroy(app.entity_manager, app.data.camera);
}

template<>
void TestAppModel::update(TestApp& app, float /*dt*/) {
	if (input::key_released(app.display, KeyCode::escape)) {
		app::quit();
	}
}

template<>
void TestAppModel::render(TestApp& app) {
	app::render_world(app.data.world, app.data.camera, "default"_viewport_name);
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
