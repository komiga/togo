#line 2 "togo/tool_build/resource_compiler/shader_def.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/utility/utility.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/string/string.hpp>
#include <togo/hash/hash.hpp>
#include <togo/kvs/kvs.hpp>
#include <togo/resource/resource.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/gfx/shader_def.hpp>
#include <togo/serialization/binary_serializer.hpp>
#include <togo/gfx/types.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/tool_build/resource_compiler.hpp>
#include <togo/tool_build/compiler_manager.hpp>

namespace togo {
namespace tool_build {

namespace resource_compiler {

namespace shader_def {

using complete_func_type = bool (
	gfx::ShaderDef& def
);

static bool read_prelude(
	gfx::ShaderDef& def,
	KVS const& k_def,
	CompilerManager& manager,
	ResourceCompilerMetadata const& metadata
) {
	KVS const* k_prelude;

	// Fetch and validate structure
	k_prelude = kvs::find(k_def, "prelude");
	if (k_prelude && !kvs::is_array(*k_prelude)) {
		TOGO_LOG_ERROR("malformed shader_def: prelude must be an array\n");
		return false;
	} else if (kvs::size(*k_prelude) > fixed_array::size(def.prelude)) {
		TOGO_LOG_ERRORF(
			"malformed shader_def: too many preludes defined (%u whereas the max is %u)\n",
			kvs::size(*k_prelude),
			static_cast<unsigned>(fixed_array::size(def.prelude))
		);
		return false;
	}

	// Read
	fixed_array::clear(def.prelude);
	for (auto const& k_dep_name : *k_prelude) {
		if (!kvs::is_string(k_dep_name) || kvs::string_size(k_dep_name) == 0) {
			TOGO_LOG_ERROR("malformed shader_def: prelude values must be non-empty strings\n");
			return false;
		}

		StringRef const dep_name{kvs::string_ref(k_dep_name)};
		ResourceNameHash const dep_name_hash = resource::hash_name(dep_name);
		if (metadata.name_hash == dep_name_hash) {
			TOGO_LOG_ERROR("malformed shader_def: definition depends on itself\n");
			return false;
		} else if (RES_NAME_SHADER_CONFIG == dep_name_hash) {
			TOGO_LOG_ERROR("malformed shader_def: shared shader config is always a dependency; remove it\n");
			return false;
		}
		if (!compiler_manager::has_resource(manager, RES_TYPE_SHADER_PRELUDE, dep_name_hash)) {
			TOGO_LOGF(
				"warning: shader dependency '%.*s' [%16lx] does not exist\n",
				dep_name.size, dep_name.data, dep_name_hash
			);
		}
		fixed_array::push_back(def.prelude, dep_name_hash);
	}
	return true;
}

static bool read_glsl_unit(
	gfx::ShaderDef& def,
	KVS const& k_def
) {
	KVS const* k_shared_source;
	KVS const* k_vertex_source;
	KVS const* k_fragment_source;

	// Fetch and validate structure
	k_shared_source = kvs::find(k_def, "shared_source");
	if (k_shared_source && !kvs::is_string(*k_shared_source)) {
		TOGO_LOG_ERROR("malformed shader_def: shared_source must be a string\n");
		return false;
	} else if (k_shared_source && kvs::string_size(*k_shared_source) == 0) {
		TOGO_LOG_ERROR("malformed shader_def: shared_source is empty\n");
		return false;
	}

	k_vertex_source = kvs::find(k_def, "vertex_source");
	if (!k_vertex_source || !kvs::is_string(*k_vertex_source)) {
		TOGO_LOG_ERROR("malformed shader_def: vertex_source not defined or not a string\n");
		return false;
	} else if (kvs::string_size(*k_vertex_source) == 0) {
		TOGO_LOG_ERROR("malformed shader_def: vertex_source is empty\n");
		return false;
	}

	k_fragment_source = kvs::find(k_def, "fragment_source");
	if (!k_fragment_source || !kvs::is_string(*k_fragment_source)) {
		TOGO_LOG_ERROR("malformed shader_def: fragment_source not defined or not a string\n");
		return false;
	} else if (kvs::string_size(*k_fragment_source) == 0) {
		TOGO_LOG_ERROR("malformed shader_def: fragment_source is empty\n");
		return false;
	}

	// Read
	def.properties |= gfx::ShaderDef::LANG_GLSL;

	u32 const capacity
		= (k_shared_source ? (kvs::string_size(*k_shared_source) + 1) : 0)
		+ kvs::string_size(*k_vertex_source) + 1
		+ kvs::string_size(*k_fragment_source) + 1
	;
	array::clear(def.data);
	array::reserve(def.data, capacity);

	// Join sources
	if (k_shared_source) {
		string::append(def.data, kvs::string_ref(*k_shared_source));
		array::back(def.data) = '\n';
	}

	def.vertex_index = array::size(def.data);
	string::append(def.data, kvs::string_ref(*k_vertex_source));
	array::back(def.data) = '\n';

	def.fragment_index = array::size(def.data);
	string::append(def.data, kvs::string_ref(*k_fragment_source));
	array::back(def.data) = '\n';

	TOGO_ASSERTE(array::size(def.data) == capacity);
	return true;
}

static bool compile(
	CompilerManager& manager,
	PackageCompiler& /*package*/,
	ResourceCompilerMetadata const& metadata,
	IReader& in_stream,
	IWriter& out_stream,
	complete_func_type& func_complete
) {
	gfx::ShaderDef def{memory::scratch_allocator()};
	KVS k_root{};
	KVS const* k_def;

	{// Read source
	ParserInfo pinfo;
	bool const read_success = kvs::read(k_root, in_stream, pinfo);
	if (!read_success) {
		TOGO_LOG_ERRORF(
			"failed to read shader_def: [%2u,%2u]: %s\n",
			pinfo.line, pinfo.column, pinfo.message
		);
		return false;
	}}

	// Fetch and validate structure
	k_def = kvs::find(k_root, "glsl");
	if (!k_def || !kvs::is_node(*k_def)) {
		TOGO_LOG_ERROR("malformed shader_def: no shader unit defined\n");
		return false;
	}

	// Read
	if (
		!read_prelude(def, *k_def, manager, metadata) ||
		!read_glsl_unit(def, *k_def) ||
		!func_complete(def)
	) {
		return false;
	}

	{// Serialize resource
	BinaryOutputSerializer ser{out_stream};
	ser % def;
	}
	return true;
}

} // namespace shader_def

namespace shader {

static bool complete(
	gfx::ShaderDef& def
) {
	def.properties |= gfx::ShaderDef::TYPE_UNIT;
	return true;
}

static bool compile(
	CompilerManager& manager,
	PackageCompiler& package,
	ResourceCompilerMetadata const& metadata,
	IReader& in_stream,
	IWriter& out_stream
) {
	return resource_compiler::shader_def::compile(
		manager, package, metadata, in_stream, out_stream,
		shader::complete
	);
}

} // namespace shader

namespace shader_prelude {

static bool complete(
	gfx::ShaderDef& def
) {
	def.properties |= gfx::ShaderDef::TYPE_PRELUDE;
	return true;
}

static bool compile(
	CompilerManager& manager,
	PackageCompiler& package,
	ResourceCompilerMetadata const& metadata,
	IReader& in_stream,
	IWriter& out_stream
) {
	return resource_compiler::shader_def::compile(
		manager, package, metadata, in_stream, out_stream,
		shader_prelude::complete
	);
}

} // namespace shader_prelude

} // namespace resource_compiler

void resource_compiler::register_shader(
	CompilerManager& cm
) {
	ResourceCompiler const compiler{
		RES_TYPE_SHADER,
		SER_FORMAT_VERSION_SHADER_DEF,
		resource_compiler::shader::compile
	};
	compiler_manager::register_compiler(cm, compiler);
}

void resource_compiler::register_shader_prelude(
	CompilerManager& cm
) {
	ResourceCompiler const compiler{
		RES_TYPE_SHADER_PRELUDE,
		SER_FORMAT_VERSION_SHADER_DEF,
		resource_compiler::shader_prelude::compile
	};
	compiler_manager::register_compiler(cm, compiler);
}

} // namespace tool_build
} // namespace togo
