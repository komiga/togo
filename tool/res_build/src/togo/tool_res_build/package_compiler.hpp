#line 2 "togo/tool_res_build/package_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief PackageCompiler interface.
@ingroup tool_res_build_package_compiler

@defgroup tool_res_build_package_compiler PackageCompiler
@ingroup tool_res_build
@details
*/

#pragma once

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/string/string.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/tool_res_build/package_compiler.gen_interface>

namespace togo {
namespace tool_res_build {
namespace package_compiler {

/**
	@addtogroup tool_res_build_package_compiler
	@{
*/

/// Name hash.
inline ResourcePackageNameHash name_hash(PackageCompiler const& pkg) {
	return pkg._name_hash;
}

/// Name.
inline StringRef name(PackageCompiler const& pkg) {
	return {pkg._name};
}

/// Path.
inline StringRef path(PackageCompiler const& pkg) {
	return {pkg._path};
}

/// Manifest.
inline Array<ResourceCompilerMetadata> const& manifest(PackageCompiler const& pkg) {
	return pkg._manifest;
}

/// Whether the package properties are marked as modified.
inline bool properties_modified(PackageCompiler const& pkg) {
	return pkg._properties_modified;
}

/// Mark the package properties as modified or unmodified.
inline void set_properties_modified(PackageCompiler& pkg, bool const value) {
	pkg._properties_modified = value;
}

/// Whether the package manifest is marked as modified.
inline bool manifest_modified(PackageCompiler const& pkg) {
	return pkg._manifest_modified;
}

/// Mark the package manifest as modified or unmodified.
inline void set_manifest_modified(PackageCompiler& pkg, bool const value) {
	pkg._manifest_modified = value;
	if (pkg._manifest_modified) {
		package_compiler::set_properties_modified(pkg, true);
		pkg._build_parity = false;
	}
}

/// Whether the package needs to be built.
inline bool needs_build(PackageCompiler const& pkg) {
	return !pkg._build_parity;
}

/// Find resource ID by path parts.
inline u32 find_resource_id(
	PackageCompiler const& pkg,
	ResourcePathParts const& path_parts,
	bool const tags_lenient
) {
	return find_resource_id(
		pkg,
		path_parts.type_hash,
		path_parts.name_hash,
		path_parts.tags_hash,
		tags_lenient
	);
}

/** @} */ // end of doc-group tool_res_build_package_compiler

} // namespace package_compiler
} // namespace tool_res_build
} // namespace togo
