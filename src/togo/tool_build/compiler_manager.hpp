#line 2 "togo/tool_build/compiler_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief CompilerManager interface.
@ingroup tool_build_compiler_manager
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/string/types.hpp>
#include <togo/resource/types.hpp>

namespace togo {
namespace tool_build {
namespace compiler_manager {

/**
	@addtogroup tool_build_compiler_manager
	@{
*/

/// Get package collection.
inline Array<PackageCompiler*> const& packages(
	CompilerManager const& cm
) {
	return cm._packages;
}

/// Add a resource compiler.
///
/// An assertion will fail if the resource type has already been
/// registered.
void register_compiler(
	CompilerManager& cm,
	ResourceCompiler const& compiler
);

/// Check if there is a compiler for type registered.
bool has_compiler(
	CompilerManager const& cm,
	ResourceType type
);

/// Find resource compiler.
ResourceCompiler const* find_compiler(
	CompilerManager const& cm,
	ResourceType type
);

/// Check if package exists.
bool has_package(
	CompilerManager const& cm,
	ResourcePackageNameHash name_hash
);

/// Find package by name hash.
PackageCompiler* find_package(
	CompilerManager& cm,
	ResourcePackageNameHash name_hash
);

/// Add package.
///
/// An assertion will fail if the package is already in the manager.
/// Returns package name hash.
ResourcePackageNameHash add_package(
	CompilerManager& cm,
	StringRef const& path
);

/// Remove package.
///
/// An assertion will fail if the package does not exist.
void remove_package(
	CompilerManager& cm,
	ResourcePackageNameHash name_hash
);

/// Remove all packages.
void clear_packages(
	CompilerManager& cm
);

/// Write all packages that are modified.
bool write_packages(
	CompilerManager& cm
);

/// Check if a resource exists.
bool has_resource(
	CompilerManager const& cm,
	ResourceType type,
	ResourceNameHash name_hash,
	ResourceTagsHash tags_hash = RES_TAGS_NULL
);

/// Find lookup node by resource name.
PackageCompiler::LookupNode* find_node(
	CompilerManager& cm,
	ResourceNameHash name_hash,
	PackageCompiler*& package
);

/** @} */ // end of doc-group tool_build_compiler_manager

} // namespace compiler_manager
} // namespace tool_build
} // namespace togo
