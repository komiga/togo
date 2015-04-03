#line 2 "togo/tool_res_build/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief tool_res_build types.
@ingroup tool_res_build_types

@defgroup tool_res_build_types Types
@ingroup tool_res_build
@details
*/

#pragma once

#include <togo/tool_res_build/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/serialization/types.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/resource/types.hpp>

namespace togo {
namespace tool_res_build {

// Forward declarations
struct PackageCompiler;
struct CompilerManager;

/**
	@addtogroup tool_res_build_types
	@{
*/

/// Format versions.
enum : u32 {
	/// PackageCompiler compiler_metadata format version.
	SER_FORMAT_VERSION_PKG_COMPILER_METADATA = 1,
};

/// Resource compiler metadata.
struct ResourceCompilerMetadata
	: ResourceMetadata
{
	u64 last_compiled;
	FixedArray<char, 256> path;
};

/** @} */ // end of doc-group tool_res_build_types

/**
	@addtogroup tool_res_build_resource_compiler
	@{
*/

/// Resource compiler.
struct ResourceCompiler {
	/// Compile a resource.
	using compile_func_type = bool (
		void* type_data,
		CompilerManager& manager,
		PackageCompiler& package,
		ResourceCompilerMetadata const& metadata,
		IReader& in_stream,
		IWriter& out_stream
	);

	ResourceType type;
	u32 format_version;
	void* type_data;
	compile_func_type* func_compile;
};

/** @} */ // end of doc-group tool_res_build_resource_compiler

/**
	@addtogroup tool_res_build_package_compiler
	@{
*/

/// Package compiler.
struct PackageCompiler {
	using LookupNode = HashMapNode<ResourceNameHash, u32>;

	// TODO: Dependency tracking
	bool _properties_modified;
	bool _manifest_modified;
	bool _build_parity;
	ResourcePackageNameHash _name_hash;
	HashMap<ResourceNameHash, u32> _lookup;
	Array<ResourceCompilerMetadata> _manifest;
	FixedArray<char, 48> _name;
	FixedArray<char, 256> _path;

	PackageCompiler() = delete;
	PackageCompiler(PackageCompiler const&) = delete;
	PackageCompiler(PackageCompiler&&) = delete;
	PackageCompiler& operator=(PackageCompiler const&) = delete;
	PackageCompiler& operator=(PackageCompiler&&) = delete;

	~PackageCompiler() = default;
	PackageCompiler(
		StringRef const& path,
		Allocator& allocator
	);
};

/** @} */ // end of doc-group tool_res_build_package_compiler

/**
	@addtogroup tool_res_build_compiler_manager
	@{
*/

/// Package and resource compiler manager.
struct CompilerManager {
	HashMap<ResourceType, ResourceCompiler> _compilers;
	Array<PackageCompiler*> _packages;

	CompilerManager() = delete;
	CompilerManager(CompilerManager const&) = delete;
	CompilerManager(CompilerManager&&) = delete;
	CompilerManager& operator=(CompilerManager const&) = delete;
	CompilerManager& operator=(CompilerManager&&) = delete;

	~CompilerManager();
	CompilerManager(Allocator& allocator);
};

/** @} */ // end of doc-group tool_res_build_compiler_manager

/**
	@addtogroup tool_res_build_gfx_compiler
	@{
*/

/// gfx::GeneratorUnit compiler.
struct GeneratorCompiler {
	/// Write a generator unit.
	///
	/// Unit data points to the KVS to translate to serial form.
	using generator_write_func_type = bool (
		GeneratorCompiler& gen_compiler,
		BinaryOutputSerializer& ser,
		gfx::GeneratorUnit const& unit
	);

	gfx::GeneratorNameHash name_hash;
	generator_write_func_type* func_write;
};

/// Graphics compiler.
struct GfxCompiler {
	HashMap<gfx::GeneratorNameHash, GeneratorCompiler> _gen_compilers;

	GfxCompiler() = delete;
	GfxCompiler(GfxCompiler const&) = delete;
	GfxCompiler(GfxCompiler&&) = delete;
	GfxCompiler& operator=(GfxCompiler const&) = delete;
	GfxCompiler& operator=(GfxCompiler&&) = delete;

	~GfxCompiler() = default;
	GfxCompiler(Allocator& allocator);
};

/** @} */ // end of doc-group tool_res_build_gfx_compiler

/**
	@addtogroup tool_res_build_interface
	@{
*/

// Tooling interface.
struct Interface {
	CompilerManager _manager;
	GfxCompiler _gfx_compiler;
	FixedArray<char, 256> _project_path;

	Interface(Interface const&) = delete;
	Interface(Interface&&) = delete;
	Interface& operator=(Interface const&) = delete;
	Interface& operator=(Interface&&) = delete;

	~Interface();
	Interface();
};

/** @} */ // end of doc-group tool_res_build_interface

} // namespace tool_res_build
} // namespace togo
