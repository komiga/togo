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
#include <togo/resource_types.hpp>

namespace togo {
namespace resource_manager {

/**
	@addtogroup resource_manager
	@{
*/

/// Register resource loader.
///
/// An assertion will fail if type has already been registered.
void register_loader(
	ResourceManager& rm,
	ResourceType type,
	void* type_data,
	ResourceLoader::load_func_type& load_func,
	ResourceLoader::unload_func_type& unload_func
);

/// Get resource.
void* get_resource(
	ResourceManager& rm,
	ResourceNameHash const name_hash
);

/** @} */ // end of doc-group resource_manager

} // namespace resource_manager
} // namespace togo
