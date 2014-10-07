#line 2 "togo/resource_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file resource_manager.hpp
@brief ResourceManager interface.
@ingroup resource
@ingroup resource_manager
*/

#pragma once

#include <togo/config.hpp>
#include <togo/string_types.hpp>
#include <togo/resource_types.hpp>

namespace togo {
namespace resource_manager {

/**
	@addtogroup resource_manager
	@{
*/

/// Package collection.
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
	ResourceHandler const& handler,
	void* type_data
);

/// Check if there is a handler for type registered.
bool has_handler(
	ResourceManager const& rm,
	ResourceType type
);

/// Add package.
///
/// Returns package hash.
hash64 add_package(
	ResourceManager& rm,
	StringRef const& root
);

/// Remove package.
void remove_package(
	ResourceManager& rm,
	hash64 root_hash
);

// Remove all packages.
void clear_packages(ResourceManager& rm);

/// Load resource.
///
/// The resource is not reloaded if it is already loaded.
void* load_resource(
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

/// Get resource.
void* get_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
);

/** @} */ // end of doc-group resource_manager

} // namespace resource_manager
} // namespace togo
