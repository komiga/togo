#line 2 "togo/resource_package.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file resource_package.hpp
@brief ResourcePackage interface.
@ingroup resource
@ingroup resource_package
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/resource_types.hpp>

namespace togo {
namespace resource_package {

/**
	@addtogroup resource_package
	@{
*/

/// Root path.
inline StringRef root(ResourcePackage const& pkg) {
	return pkg._root;
}

/// Hash of root path (cached).
inline hash64 root_hash(ResourcePackage const& pkg) {
	return pkg._root_hash;
}

/// Load package manifest.
void load_manifest(
	ResourcePackage& pkg,
	ResourceManager const& rm
);

/// Find resource.
ResourcePackage::EntryNode* find_resource(
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
	ResourcePackage::EntryNode* node
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
