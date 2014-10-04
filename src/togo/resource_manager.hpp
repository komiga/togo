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

/// Register resource handler.
///
/// An assertion will fail if a handler for the type has already
/// been registered.
void register_handler(
	ResourceManager& rm,
	ResourceHandler const& handler,
	void* type_data
);

/// Get resource.
void* get_resource(
	ResourceManager& rm,
	ResourceNameHash const name_hash
);

/** @} */ // end of doc-group resource_manager

} // namespace resource_manager
} // namespace togo
