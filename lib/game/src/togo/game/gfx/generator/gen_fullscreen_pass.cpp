#line 2 "togo/game/gfx/generator/gen_fullscreen_pass.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/gfx/generator.hpp>
#include <togo/game/gfx/render_node.hpp>
#include <togo/game/gfx/renderer.hpp>
#include <togo/game/resource/resource_handler.hpp>
#include <togo/game/app/app.hpp>

namespace togo {
namespace game {
namespace gfx {

namespace generator {
namespace gen_fullscreen_pass {

struct UnitData {
	gfx::CmdRenderFullscreenPass cmd;
};

struct DefData {
	Array<UnitData> unit_storage;
};

static void init(
	gfx::GeneratorDef& def,
	gfx::Renderer* /*renderer*/
) {
	if (!def.data) {
		def.data = TOGO_CONSTRUCT(
			memory::default_allocator(), DefData, {
			{memory::default_allocator()}
		});
	} else {
		// Reinitialization
		auto def_data = static_cast<DefData*>(def.data);
		array::clear(def_data->unit_storage);
	}
}

static void destroy(
	gfx::GeneratorDef const& def,
	gfx::Renderer* /*renderer*/
) {
	auto def_data = static_cast<DefData*>(def.data);
	TOGO_DESTROY(memory::default_allocator(), def_data);
}

static void exec(
	gfx::GeneratorUnit const& unit,
	gfx::RenderNode& node,
	gfx::RenderObject const* const /*objects_begin*/,
	gfx::RenderObject const* const /*objects_end*/
) {
	auto def_data = static_cast<DefData*>(unit.data);
	auto& data = def_data->unit_storage[unit.data_index];
	gfx::render_node::push(node, 0, data.cmd);
}

static void read(
	gfx::GeneratorDef const& def,
	gfx::Renderer* renderer,
	BinaryInputSerializer& ser,
	gfx::GeneratorUnit& unit
) {
	auto& app = app::instance();
	auto def_data = static_cast<DefData*>(def.data);

	struct {
		ResourceNameHash shader_name_hash;
		u32 input_index;
		u32 output_index;
	} ser_data;
	ser
		% ser_data.shader_name_hash
		% ser_data.input_index
		% ser_data.output_index
	;

	UnitData unit_data;
	unit_data.cmd.shader_id = resource::load_shader(
		app.resource_manager, ser_data.shader_name_hash
	);

	auto input_id = gfx::renderer::shared_render_target(renderer, ser_data.input_index);
	unit_data.cmd.framebuffer_id = gfx::renderer::create_framebuffer(
		renderer, 1, &input_id, {}
	);
	unit_data.cmd.output_id = gfx::renderer::shared_render_target(renderer, ser_data.output_index);

	unit.data = def_data;
	unit.data_index = array::size(def_data->unit_storage);
	array::push_back(def_data->unit_storage, unit_data);
}

} // namespace gen_fullscreen_pass
} // namespace generator

gfx::GeneratorDef const generator::fullscreen_pass{
	"fullscreen_pass"_generator_name,
	nullptr,
	generator::gen_fullscreen_pass::init,
	generator::gen_fullscreen_pass::destroy,
	generator::gen_fullscreen_pass::read,
	generator::gen_fullscreen_pass::exec
};

} // namespace gfx
} // namespace game
} // namespace togo
