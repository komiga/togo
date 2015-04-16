#line 2 "togo/tool_res_build/generator_compiler/gc_clear.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/gfx.hpp>
#include <togo/tool_res_build/generator_compiler.hpp>

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

	{// Read render_target
	StringRef const rt_name = k_rt ? kvs::string_ref(*k_rt) : "back_buffer";
	hash32 const rt_name_hash = hash::calc32(rt_name);
	if (rt_name_hash != "back_buffer"_hash32) {
		rt_index = 0;
		for (auto const& resource : render_config.shared_resources) {
			if (resource.name_hash == rt_name_hash) {
				if (resource.type() == gfx::RenderConfigResource::TYPE_RENDER_TARGET) {
					goto l_found;
				} else {
					TOGO_LOG_ERRORF(
						"malformed generator: clear: "
						"render_target '%.*s' is not a render target\n",
						rt_name.size, rt_name.data
					);
				}
			} else if (resource.type() == gfx::RenderConfigResource::TYPE_RENDER_TARGET) {
				++rt_index;
			}
		}
		TOGO_LOG_ERRORF(
			"malformed generator: clear: "
			"render_target '%.*s' not found\n",
			rt_name.size, rt_name.data
		);

		l_found:
		(void)0;
	}}

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
