#line 2 "togo/tool_res_build/generator_compiler/gc_fullscreen_pass.cpp"
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
#include <togo/game/resource/resource.hpp>
#include <togo/tool_res_build/generator_compiler.hpp>
#include <togo/tool_res_build/generator_compiler/support.hpp>

namespace togo {
namespace tool_res_build {

namespace generator_compiler {
namespace gc_fullscreen_pass {

static bool write(
	GeneratorCompiler& /*gen_compiler*/,
	BinaryOutputSerializer& ser,
	gfx::RenderConfig const& render_config,
	gfx::GeneratorUnit const& unit
) {
	auto* const k_root = static_cast<KVS const*>(unit.data);

	// Fetch and validate structure
	auto* const k_shader = kvs::find(*k_root, "shader");
	if (!k_shader || !kvs::is_type(*k_shader, KVSType::string) || !kvs::string_size(*k_shader)) {
		TOGO_LOG_ERROR(
			"malformed generator: fullscreen_pass: "
			"'shader' is either not a string or is empty\n"
		);
		return false;
	}

	auto* const k_input = kvs::find(*k_root, "input");
	if (!k_input || !kvs::is_type(*k_input, KVSType::string) || !kvs::string_size(*k_input)) {
		TOGO_LOG_ERROR(
			"malformed generator: fullscreen_pass: "
			"'input' is either not a string or is empty\n"
		);
		return false;
	}

	auto* const k_output = kvs::find(*k_root, "output");
	if (!k_output || !kvs::is_type(*k_output, KVSType::string) || !kvs::string_size(*k_output)) {
		TOGO_LOG_ERROR(
			"malformed generator: fullscreen_pass: "
			"'output' is either not a string or is empty\n"
		);
		return false;
	}

	struct {
		ResourceNameHash shader_name_hash;
		u32 input_index;
		u32 output_index;
	} ser_data;

	// Read properties
	ser_data.shader_name_hash = resource::hash_name(kvs::string_ref(*k_shader));
	if (
		!rc_get_render_target("fullscreen_pass", render_config, *k_input, ser_data.input_index) ||
		!rc_get_render_target("fullscreen_pass", render_config, *k_output, ser_data.output_index)
	) {
		return false;
	}

	// Validate
	if (ser_data.input_index == ser_data.output_index) {
		auto& rt = render_config.shared_resources[ser_data.input_index].data.render_target;
		if (~rt.properties & gfx::RenderTargetSpec::F_DOUBLE_BUFFERED) {
			TOGO_LOG_ERRORF(
				"malformed generator: fullscreen_pass: "
				"render target '%.*s' must be double-buffered when used as both input and output\n",
				kvs::string_size(*k_input), kvs::string(*k_input)
			);
			return false;
		}
	}

	ser
		% ser_data.shader_name_hash
		% ser_data.input_index
		% ser_data.output_index
	;
	return true;
}

} // namespace gc_fullscreen_pass
} // namespace generator_compiler

GeneratorCompiler const generator_compiler::fullscreen_pass{
	gfx::hash_generator_name("fullscreen_pass"),
	generator_compiler::gc_fullscreen_pass::write
};

} // namespace tool_res_build
} // namespace togo
