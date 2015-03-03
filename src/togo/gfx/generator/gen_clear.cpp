#line 2 "togo/gfx/generator/gen_clear.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/hash/hash.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/generator.hpp>
#include <togo/gfx/render_node.hpp>
#include <togo/gfx/renderer.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/binary_serializer.hpp>

namespace togo {
namespace gfx {

namespace generator {
namespace gen_clear {

union Data {
	void* ptr;
	hash32 rt;
};

static void exec(
	gfx::GeneratorUnit const& unit,
	gfx::RenderNode& node,
	gfx::RenderObject const* const /*objects_begin*/,
	gfx::RenderObject const* const /*objects_end*/
) {
	// TODO: Command for clearing any RT instead?
	// TODO: Clear all RTs in layer?
	Data const data{unit.data};
	hash32 const rt = data.rt;
	TOGO_ASSERTE(rt == "back_buffer"_hash32);
	gfx::render_node::push(node, 0, gfx::CmdClearBackbuffer{});
}

static void read(
	gfx::GeneratorDef const& /*def*/,
	gfx::Renderer* /*renderer*/,
	BinaryInputSerializer& ser,
	gfx::GeneratorUnit& unit
) {
	Data data;
	ser % data.rt;
	unit.data = data.ptr;
	unit.func_exec = exec;
}

} // namespace gen_clear
} // namespace generator

gfx::GeneratorDef const generator::clear{
	"clear"_generator_name,
	nullptr,
	nullptr,
	generator::gen_clear::read
};

} // namespace gfx
} // namespace togo
