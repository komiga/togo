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

/// Get manifest.
inline Array<ResourceCompilerMetadata> const& manifest(
	PackageCompiler const& pkg
) {
	return pkg._manifest;
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
	if (pkg._manifest_modified) {
		package_compiler::set_properties_modified(pkg, pkg._build_parity != false || true);
		pkg._build_parity = false;
	}
}

/// Check if the package needs to be built.
inline bool needs_build(
	PackageCompiler const& pkg
) {
	return !pkg._build_parity;
}

/// Get lookup node by resource name.
PackageCompiler::LookupNode* get_node(
	PackageCompiler& pkg,
	ResourceNameHash name_hash
);

/// Find resource ID by identity.
///
/// 0 is returned if a resource was not found.
/// If tags_lenient is true, a tag-less entry will match if there is
/// no resource with tags_hash exactly.
u32 find_resource_id(
	PackageCompiler const& pkg,
	ResourceType type,
	ResourceNameHash name_hash,
	ResourceTagsHash tags_hash,
	bool tags_lenient
);

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

/// Build package.
bool build(
	PackageCompiler& pkg,
	StringRef const& output_path
);

/** @} */ // end of doc-group tool_build_package_compiler

} // namespace package_compiler
} // namespace tool_build
} // namespace togo
