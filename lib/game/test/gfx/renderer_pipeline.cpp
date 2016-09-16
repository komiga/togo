
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/vector/2_type.hpp>
#include <togo/core/math/vector/3_type.hpp>
#include <togo/core/log/log.hpp>
#include <togo/window/window/window.hpp>
#include <togo/window/input/input.hpp>
#include <togo/game/entity/entity_manager.hpp>
#include <togo/game/world/world_manager.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/game/gfx/command.hpp>
#include <togo/game/gfx/render_node.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/gfx/renderer/private.hpp>
#include <togo/game/gfx/renderer/opengl.hpp>
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
	gfx::BufferID buffer;
	gfx::BufferBindingID binding;
	gfx::ShaderID shader;
};

void gen_test_proxy_init(
	gfx::GeneratorDef& def,
	gfx::Renderer* renderer
) {
	if (def.data) {
		return;
	}
	auto& app = app::instance();
	auto* data = TOGO_CONSTRUCT_DEFAULT(
		memory::default_allocator(), GenTestProxyData
	);

	data->buffer = gfx::renderer::create_buffer(
		renderer, sizeof(triangle_vertices), triangle_vertices
	);
	data->binding = gfx::renderer::create_buffer_binding(
		renderer, array_extent(triangle_vertices), 0, {},
		{{data->buffer, &triangle_vformat, 0}}
	);
	data->shader = resource::ref_shader(
		app.resource_manager,
		"test/gfx/generic"_resource_name
	);
	def.data = data;
}

void gen_test_proxy_destroy(
	gfx::GeneratorDef const& def,
	gfx::Renderer* renderer
) {
	auto& app = app::instance();
	auto* data = static_cast<GenTestProxyData*>(def.data);
	resource::unref_shader(app.resource_manager, "test/gfx/generic"_resource_name);
	gfx::renderer::destroy_buffer_binding(renderer, data->binding);
	gfx::renderer::destroy_buffer(renderer, data->buffer);
	TOGO_DESTROY(memory::default_allocator(), data);
}

void gen_test_proxy_read(
	gfx::GeneratorDef const& def,
	gfx::Renderer* /*renderer*/,
	BinaryInputSerializer& /*ser*/,
	gfx::GeneratorUnit& unit
) {
	unit.data = def.data;
}

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

struct TestAppData {
	WorldID world;
	EntityID camera;
};

using TestApp = App<TestAppData>;
using TestAppModel = AppModel<TestAppData>;

template<>
void TestAppModel::init(TestApp& app) {
	resource_manager::add_package(app.resource_manager, "test_data");
	window::set_swap_mode(app.window, WindowSwapMode::wait_refresh);

	gfx::renderer::register_generator_def(
		app.renderer,
		gfx::GeneratorDef{
			"test_proxy"_generator_name,
			nullptr,
			gen_test_proxy_init,
			gen_test_proxy_destroy,
			gen_test_proxy_read,
			gen_test_proxy_exec
		}
	);
	auto* render_config = resource::ref_render_config(
		app.resource_manager,
		"test/gfx/pipeline"_resource_name
	);
	gfx::renderer::configure(app.renderer, *render_config);
	resource::unref_render_config(app.resource_manager, "test/gfx/pipeline"_resource_name);

	app.data.world = world_manager::create(app.world_manager);
	app.data.camera = entity_manager::create(app.entity_manager);
}

template<>
void TestAppModel::shutdown(TestApp& app) {
	entity_manager::destroy(app.entity_manager, app.data.camera);
	world_manager::destroy(app.world_manager, app.data.world);
}

template<>
void TestAppModel::update(TestApp& app, float /*dt*/) {
	if (input::key_released(app.window, KeyCode::escape)) {
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
		array_ref(argv, unsigned_cast(argc)),
		"data/project/package/",
		1.0f / 30.0f
	);
	app::run();
	app::shutdown();
	return 0;
}
