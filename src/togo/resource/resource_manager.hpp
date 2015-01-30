#line 2 "togo/resource/resource_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceManager interface.
@ingroup resource
@ingroup resource_manager
*/

#pragma once

#include <togo/config.hpp>
#include <togo/string/types.hpp>
#include <togo/resource/types.hpp>

namespace togo {
namespace resource_manager {

/**
	@addtogroup resource_manager
	@{
*/

/// Get base path.
inline StringRef base_path(
	ResourceManager const& rm
) {
	return rm._base_path;
}

/// Get package collection.
inline Array<ResourcePackage*> const& packages(
	ResourceManager const& rm
) {
	return rm._packages;
}

/// Register resource handler.
///
/// An assertion will fail if a handler for the type has already
/// been registered.
void register_handler(
	ResourceManager& rm,
	ResourceHandler const& handler
);

/// Remove all resource handlers.
void clear_handlers(ResourceManager& rm);

/// Check if a resource handler is registered.
bool has_handler(
	ResourceManager const& rm,
	ResourceType type
);

/// Add package by name and base path.
///
/// Returns package name hash.
ResourcePackageNameHash add_package(
	ResourceManager& rm,
	StringRef const& name
);

/// Add package by name and path.
///
/// Returns package name hash.
ResourcePackageNameHash add_package(
	ResourceManager& rm,
	StringRef const& name,
	StringRef const& path
);

/// Remove package.
void remove_package(
	ResourceManager& rm,
	ResourcePackageNameHash name_hash
);

/// Remove all packages.
void clear_packages(ResourceManager& rm);

/// Check if a resource exists.
bool has_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
);

/// Load resource.
///
/// The resource is not reloaded if it is already loaded.
ResourceValue load_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
);

/// Unload resource.
void unload_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
);

/// Unload all resources.
void clear_resources(ResourceManager& rm);

/// Find resource by type and name.
ResourceValue find_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
);

/** @} */ // end of doc-group resource_manager

} // namespace resource_manager
} // namespace togo
