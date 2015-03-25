#line 2 "togo/tool_build/resource_compiler/shader_def.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/string/string.hpp>
#include <togo/hash/hash.hpp>
#include <togo/kvs/kvs.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/gfx.hpp>
#include <togo/gfx/shader_def.hpp>
#include <togo/resource/resource.hpp>
#include <togo/serialization/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/binary_serializer.hpp>
#include <togo/serialization/gfx/shader_def.hpp>
#include <togo/tool_build/resource_compiler.hpp>
#include <togo/tool_build/compiler_manager.hpp>

namespace togo {
namespace tool_build {

namespace resource_compiler {

namespace shader_def {

using complete_func_type = bool (
	gfx::ShaderDef& def
);

using read_unit_func_type = bool (
	gfx::ShaderDef& def,
	KVS const& k_def
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

static bool read_param_blocks(
	KVS const& k_def,
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND>& param_blocks,
	StringRef const param_blocks_name,
	bool const indexed
) {
	KVS const* k_param_blocks;

	// Fetch and validate structure
	k_param_blocks = kvs::find(k_def, param_blocks_name);
	if (k_param_blocks && !kvs::is_array(*k_param_blocks)) {
		TOGO_LOG_ERRORF(
			"malformed shader_def: %.*s must be an array\n",
			param_blocks_name.size, param_blocks_name.data
		);
		return false;
	} else if (!k_param_blocks) {
		return true;
	}

	{// Read
	KVS const* k_name;
	KVS const* k_index;
	unsigned local_index = 0;
	for (KVS const& k_pb_def : *k_param_blocks) {
		if (!kvs::is_node(k_pb_def)) {
			TOGO_LOG_ERRORF(
				"malformed shader_def: %.*s values must be nodes\n",
				param_blocks_name.size, param_blocks_name.data
			);
			return false;
		}

		// Fetch and validate structure
		k_name = kvs::find(k_pb_def, "name");
		if (!k_name || !kvs::is_string(*k_name) || kvs::string_size(*k_name) == 0) {
			TOGO_LOG_ERROR("malformed shader_def: param block definition: name must be a non-empty string\n");
			return false;
		}
		k_index = kvs::find(k_pb_def, "index");
		if (!indexed && k_index) {
			TOGO_LOG_ERRORF(
				"malformed shader_def: %.*s definitions cannot be explicitly indexed\n",
				param_blocks_name.size, param_blocks_name.data
			);
			return false;
		} else if (
			indexed && (
				!k_index ||
				!kvs::is_integer(*k_index) ||
				kvs::integer(*k_index) < 0 ||
				kvs::integer(*k_index) > 15
			)
		) {
			TOGO_LOG_ERRORF(
				"malformed shader_def: %.*s definitions must be explicitly indexed in [0, 15]\n",
				param_blocks_name.size, param_blocks_name.data
			);
			return false;
		}

		{// Read
		StringRef const name = kvs::string_ref(*k_name);
		fixed_array::push_back(
			param_blocks,
			{
				indexed
				? static_cast<unsigned>(kvs::integer(*k_index))
				: local_index,
				gfx::hash_param_block_name(name),
				name
			}
		);
		}
		++local_index;
	}}
	return true;
}

inline static void sum_data_table_capacities(
	unsigned& capacity,
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND>& param_blocks
) {
	for (auto const& pb_def : param_blocks) {
		// + NUL
		capacity += pb_def.name.size + 1;
	}
}

inline static void push_param_block_names(
	gfx::ShaderDef& def,
	FixedArray<gfx::ParamBlockDef, TOGO_GFX_NUM_PARAM_BLOCKS_BY_KIND>& param_blocks
) {
	for (auto const& pb_def : param_blocks) {
		fixed_array::push_back(def.data_offsets, static_cast<u32>(array::size(def.data)));
		string::append(def.data, pb_def.name);
		array::back(def.data) = '\x03';
	}
}

inline static bool check_source(
	KVS const* k_source
) {
	if (!k_source || !kvs::is_string(*k_source)) {
		TOGO_LOG_ERRORF(
			"malformed shader_def: '%.*s' not defined or not a string\n",
			kvs::name_size(*k_source), kvs::name(*k_source)
		);
		return false;
	} else if (kvs::string_size(*k_source) == 0) {
		TOGO_LOG_ERRORF(
			"malformed shader_def: '%.*s' is empty\n",
			kvs::name_size(*k_source), kvs::name(*k_source)
		);
		return false;
	}
	return true;
}

static bool read_glsl_unit(
	gfx::ShaderDef& def,
	KVS const& k_def
) {
	static constexpr StringRef const source_names[]{
		"shared_source",
		"vertex_source",
		"fragment_source",
	};
	enum : unsigned { NUM_SOURCES = 1 + unsigned_cast(gfx::ShaderStage::Type::NUM) };
	KVS const* k_sources[NUM_SOURCES];

	// Fetch and validate structure
	for (unsigned index = 0; index < array_extent(k_sources); ++index) {
		k_sources[index] = kvs::find(k_def, source_names[index]);
	}
	if (
		!check_source(k_sources[1 + unsigned_cast(gfx::ShaderStage::Type::vertex)]) ||
		!check_source(k_sources[1 + unsigned_cast(gfx::ShaderStage::Type::fragment)])
	) {
		return false;
	}

	// Read
	def.properties |= gfx::ShaderDef::LANG_GLSL;

	// Read param blocks
	fixed_array::clear(def.fixed_param_blocks);
	fixed_array::clear(def.draw_param_blocks);
	if (
		!read_param_blocks(k_def, def.fixed_param_blocks, "fixed_param_blocks", true) ||
		!read_param_blocks(k_def, def.draw_param_blocks, "draw_param_blocks", false)
	) {
		return false;
	}

	// Source capacity
	u32 capacity = 0;
	for (KVS const* k_source : k_sources) {
		if (k_source) {
			// + \n
			capacity += kvs::string_size(*k_source) + 1;
		}
	}

	// Param block names capacity
	sum_data_table_capacities(capacity, def.fixed_param_blocks);
	sum_data_table_capacities(capacity, def.draw_param_blocks);

	array::clear(def.data);
	array::reserve(def.data, capacity);
	fixed_array::clear(def.data_offsets);

	// Push sources
	for (unsigned index = 0; index < NUM_SOURCES; ++index) {
		KVS const* const k_source = k_sources[index];
		if (index != 0) {
			fixed_array::push_back(def.data_offsets, static_cast<u32>(array::size(def.data)));
		}
		if (k_source) {
			string::append(def.data, kvs::string_ref(*k_source));
			array::back(def.data) = '\n';
		}
	}

	// Push param block names
	push_param_block_names(def, def.fixed_param_blocks);
	push_param_block_names(def, def.draw_param_blocks);

	// Endcap
	fixed_array::push_back(def.data_offsets, static_cast<u32>(array::size(def.data)));
	TOGO_ASSERTE(array::size(def.data) == capacity);

	// Replace \x03 with NUL for param block name termination
	for (
		unsigned index = gfx::ShaderDef::IDX_PARAM_NAMES + 1;
		index < fixed_array::size(def.data_offsets);
		++index
	) {
		def.data[def.data_offsets[index] - 1] = '\0';
	}

	// Link param block names to def.data
	gfx::shader_def::patch_param_block_names(def);

	return true;
}

static bool compile(
	void* /*type_data*/,
	CompilerManager& manager,
	PackageCompiler& /*package*/,
	ResourceCompilerMetadata const& metadata,
	IReader& in_stream,
	IWriter& out_stream,
	complete_func_type& func_complete
) {
	gfx::ShaderDef def{memory::scratch_allocator()};
	KVS k_root{};
	KVS const* k_type;

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
	k_type = kvs::find(k_root, "type");
	if (!k_type || !kvs::is_string(*k_type)) {
		TOGO_LOG_ERROR("malformed shader_def: type not specified\n");
		return false;
	}

	read_unit_func_type* func_read_unit;
	switch (hash::calc32(kvs::string_ref(*k_type))) {
	case "glsl"_hash32: func_read_unit = read_glsl_unit; break;
	default:
		TOGO_LOG_ERRORF(
			"malformed shader_def: unknown type '%.*s'\n",
			kvs::string_size(*k_type), kvs::string(*k_type)
		);
		return false;
	}

	// Read
	if (
		!read_prelude(def, k_root, manager, metadata) ||
		!func_read_unit(def, k_root) ||
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
	void* type_data,
	CompilerManager& manager,
	PackageCompiler& package,
	ResourceCompilerMetadata const& metadata,
	IReader& in_stream,
	IWriter& out_stream
) {
	return resource_compiler::shader_def::compile(
		type_data, manager, package, metadata, in_stream, out_stream,
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
	void* type_data,
	CompilerManager& manager,
	PackageCompiler& package,
	ResourceCompilerMetadata const& metadata,
	IReader& in_stream,
	IWriter& out_stream
) {
	return resource_compiler::shader_def::compile(
		type_data, manager, package, metadata, in_stream, out_stream,
		shader_prelude::complete
	);
}

} // namespace shader_prelude

} // namespace resource_compiler

/// Register shader_prelude resource compiler.
void resource_compiler::register_shader_prelude(
	CompilerManager& cm
) {
	ResourceCompiler const compiler{
		RES_TYPE_SHADER_PRELUDE,
		SER_FORMAT_VERSION_SHADER_DEF,
		nullptr,
		resource_compiler::shader_prelude::compile
	};
	compiler_manager::register_compiler(cm, compiler);
}

/// Register shader resource compiler.
void resource_compiler::register_shader(
	CompilerManager& cm
) {
	ResourceCompiler const compiler{
		RES_TYPE_SHADER,
		SER_FORMAT_VERSION_SHADER_DEF,
		nullptr,
		resource_compiler::shader::compile
	};
	compiler_manager::register_compiler(cm, compiler);
}

} // namespace tool_build
} // namespace togo
