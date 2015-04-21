#line 2 "togo/tool_res_build/generator_compiler/gc_clear.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/tool_res_build/generator_compiler.hpp>
#include <togo/tool_res_build/generator_compiler/support.hpp>

namespace togo {
namespace tool_res_build {

namespace generator_compiler {
namespace gc_clear {

static bool write(
	GeneratorCompiler& /*gen_compiler*/,
	BinaryOutputSerializer& ser,
	gfx::RenderConfig const& render_config,
	gfx::GeneratorUnit const& unit
) {
	auto* const k_root = static_cast<KVS const*>(unit.data);

	// Fetch and validate structure
	auto* const k_rt = kvs::find(*k_root, "render_target");
	if (k_rt && (!kvs::is_type(*k_rt, KVSType::string) || !kvs::string_size(*k_rt))) {
		TOGO_LOG_ERROR(
			"malformed generator: clear: "
			"'render_target' is either not a string or is empty\n"
		);
		return false;
	}

	// Generator data
	u32 rt_index = ~u32{0};

	// Read render_target
	StringRef const rt_name = k_rt ? kvs::string_ref(*k_rt) : "back_buffer";
	if (!string::compare_equal(rt_name, "back_buffer")) {
		if (!rc_get_render_target("clear", render_config, *k_rt, rt_index)) {
			return false;
		}
	}

	ser % rt_index;
	return true;
}

} // namespace gc_clear
} // namespace generator_compiler

GeneratorCompiler const generator_compiler::clear{
	gfx::hash_generator_name("clear"),
	generator_compiler::gc_clear::write
};

} // namespace tool_res_build
} // namespace togo
