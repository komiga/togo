#line 2 "togo/tool_build/package_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief PackageCompiler interface.
@ingroup tool_build_package_compiler
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/string_types.hpp>
#include <togo/string.hpp>
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

/// Find lookup node by resource name.
PackageCompiler::LookupNode* get_node(
	PackageCompiler& pkg,
	ResourceNameHash name_hash
);

/// Read package data.
void read(
	PackageCompiler& pkg
);

/// Write package data.
bool write(
	PackageCompiler& pkg
);

/** @} */ // end of doc-group tool_build_package_compiler

} // namespace package_compiler
} // namespace tool_build
} // namespace togo
