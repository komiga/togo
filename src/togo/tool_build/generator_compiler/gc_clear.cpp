#line 2 "togo/tool_build/generator_compiler/gc_clear.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/utility/utility.hpp>
#include <togo/log/log.hpp>
#include <togo/hash/hash.hpp>
#include <togo/kvs/kvs.hpp>
#include <togo/gfx/gfx.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/binary_serializer.hpp>
#include <togo/tool_build/generator_compiler.hpp>

namespace togo {
namespace tool_build {

namespace generator_compiler {
namespace gc_clear {

static bool write(
	GeneratorCompiler& /*gen_compiler*/,
	BinaryOutputSerializer& ser,
	gfx::GeneratorUnit const& unit
) {
	auto* const k_root = static_cast<KVS const*>(unit.data);

	// Fetch and validate structure
	auto* const k_rt = kvs::find(*k_root, "rt");
	if (k_rt && (!kvs::is_type(*k_rt, KVSType::string) || !kvs::string_size(*k_rt))) {
		TOGO_LOG_ERROR("malformed generator: clear: 'rt' is either not a string or is empty\n");
		return false;
	}

	// TODO: Validate RT?
	hash32 const rt = k_rt ? hash::calc32(kvs::string_ref(*k_rt)) : "back_buffer"_hash32;
	ser % rt;
	return true;
}

} // namespace gc_clear
} // namespace generator_compiler

GeneratorCompiler const generator_compiler::clear{
	gfx::hash_generator_name("clear"),
	generator_compiler::gc_clear::write
};

} // namespace tool_build
} // namespace togo
