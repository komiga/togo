#line 2 "togo/tool_res_build/resource_compiler/test_resource.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/hash_map.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/serialization/resource/test_resource.hpp>
#include <togo/tool_res_build/resource_compiler.hpp>
#include <togo/tool_res_build/compiler_manager.hpp>

namespace togo {
namespace tool_res_build {

namespace resource_compiler {
namespace test_resource {

static bool compile(
	void* /*type_data*/,
	CompilerManager& /*manager*/,
	PackageCompiler& /*package*/,
	ResourceCompilerMetadata const& /*metadata*/,
	IReader& in_stream,
	IWriter& out_stream
) {
	KVS k_root{};

	{// Read source
	KVSParserInfo pinfo;
	bool const read_success = kvs::read_text(k_root, in_stream, pinfo);
	if (!read_success) {
		TOGO_LOG_ERRORF(
			"failed to read test_resource: [%2u,%2u]: %s\n",
			pinfo.line, pinfo.column, pinfo.message
		);
		return false;
	}}

	// Fetch and validate structure
	KVS const* const k_x = kvs::find(k_root, "x");
	if (!k_x || !kvs::is_integer(*k_x)) {
		TOGO_LOG_ERROR("malformed test_resource: x is not defined\n");
		return false;
	}

	// Read resource
	TestResource test_resource{0};
	test_resource.x = kvs::integer(*k_x);

	{// Serialize resource
	BinaryOutputSerializer ser{out_stream};
	ser % test_resource;
	}
	return true;
}

} // namespace test_resource
} // namespace resource_compiler

/// Register test resource compiler.
void resource_compiler::register_test(
	CompilerManager& cm
) {
	ResourceCompiler const compiler{
		RES_TYPE_TEST,
		SER_FORMAT_VERSION_TEST_RESOURCE,
		nullptr,
		resource_compiler::test_resource::compile
	};
	compiler_manager::register_compiler(cm, compiler);
}

} // namespace tool_res_build
} // namespace togo
