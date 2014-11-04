#line 2 "togo/tool_build/package_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief PackageCompiler interface.
@ingroup tool_build_package_compiler
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/string/types.hpp>
#include <togo/string/string.hpp>
#include <togo/resource/types.hpp>
#include <togo/tool_build/types.hpp>

namespace togo {
namespace tool_build {
namespace package_compiler {

/**
	@addtogroup tool_build_package_compiler
	@{
*/

/// Create a package stub.
///
/// This will fail if the path already exists or if any part of the
/// package couldn't be created.
bool create_stub(
	StringRef const& path,
	StringRef const& name
);

/// Get name hash.
inline ResourcePackageNameHash name_hash(
	PackageCompiler const& pkg
) {
	return pkg._name_hash;
}

/// Get name.
inline StringRef name(
	PackageCompiler const& pkg
) {
	return {pkg._name};
}

/// Get path.
inline StringRef path(
	PackageCompiler const& pkg
) {
	return {pkg._path};
}

/// Get resource metadata.
inline Array<ResourceMetadata> const& metadata(
	PackageCompiler const& pkg
) {
	return pkg._metadata;
}

/// Check if the package properties are marked as modified.
inline bool properties_modified(
	PackageCompiler const& pkg
) {
	return pkg._properties_modified;
}

/// Mark the package properties as modified or unmodified.
inline void set_properties_modified(
	PackageCompiler& pkg,
	bool const value
) {
	pkg._properties_modified = value;
}

/// Check if the package manifest is marked as modified.
inline bool manifest_modified(
	PackageCompiler const& pkg
) {
	return pkg._manifest_modified;
}

/// Mark the package manifest as modified or unmodified.
inline void set_manifest_modified(
	PackageCompiler& pkg,
	bool const value
) {
	pkg._manifest_modified = value;
}

/// Find lookup node by resource name.
PackageCompiler::LookupNode* get_node(
	PackageCompiler& pkg,
	ResourceNameHash name_hash
);

/// Find resource ID by path parts.
///
/// If this returns 0, the resource was not found.
u32 find_resource_id(
	PackageCompiler const& pkg,
	ResourcePathParts const& path_parts
);

/// Add resource.
u32 add_resource(
	PackageCompiler& pkg,
	StringRef const& path,
	ResourcePathParts const& path_parts
);

/// Remove resource by ID.
///
/// This assumes the working directory is already at the package.
void remove_resource(
	PackageCompiler& pkg,
	u32 id
);

/// Read package data.
void read(
	PackageCompiler& pkg
);

/// Write modified package data and mark it as not modified.
bool write(
	PackageCompiler& pkg
);

/// Write package properties.
bool write_properties(
	PackageCompiler& pkg
);

/// Write package manifest.
bool write_manifest(
	PackageCompiler& pkg
);

/** @} */ // end of doc-group tool_build_package_compiler

} // namespace package_compiler
} // namespace tool_build
} // namespace togo
