#line 2 "togo/resource/resource_package.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourcePackage interface.
@ingroup resource
@ingroup resource_package
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/resource/types.hpp>

namespace togo {
namespace resource_package {

/**
	@addtogroup resource_package
	@{
*/

/// Get name hash (cached).
inline ResourcePackageNameHash name_hash(ResourcePackage const& pkg) {
	return pkg._name_hash;
}

/// Get name.
inline StringRef name(ResourcePackage const& pkg) {
	return pkg._name;
}

/// Get path.
inline StringRef path(ResourcePackage const& pkg) {
	return pkg._path;
}

/// Open package.
void open(
	ResourcePackage& pkg,
	ResourceManager const& rm
);

/// Close package.
void close(
	ResourcePackage& pkg
);

/// Get resource metadata by ID.
///
/// An assertion will fail if the ID is invalid.
ResourceMetadata const& resource_metadata(
	ResourcePackage const& pkg,
	u32 id
);

/// Find lookup node by resource name.
ResourcePackage::LookupNode* find_node(
	ResourcePackage& pkg,
	ResourceNameHash name_hash
);

/// Open resource stream by ID.
///
/// An assertion will fail if resource stream couldn't be opened.
/// An assertion will fail if there is already an open stream.
IReader* open_resource_stream(
	ResourcePackage& pkg,
	u32 id
);

/// Close current resource stream.
///
/// An assertion will fail if there is no open stream.
void close_resource_stream(
	ResourcePackage& pkg
);

/** @} */ // end of doc-group resource_package

} // namespace resource_package
} // namespace togo
