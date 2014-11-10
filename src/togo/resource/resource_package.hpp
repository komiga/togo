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

/// Find resource.
ResourcePackage::LookupNode* find_resource(
	ResourcePackage& pkg,
	ResourceType type,
	ResourceNameHash name_hash
);

/// Open resource stream.
///
/// If the resource stream couldn't be opened, this returns nullptr.
/// An assertion will fail if node is nullptr.
/// An assertion will fail if there is already an open stream.
IReader* open_resource_stream(
	ResourcePackage& pkg,
	ResourcePackage::LookupNode* node
);

/// Close resource stream.
///
/// This closes the previously-opened resource stream.
/// An assertion will fail if there is no open stream.
void close_resource_stream(
	ResourcePackage& pkg
);

/** @} */ // end of doc-group resource_package

} // namespace resource_package
} // namespace togo
