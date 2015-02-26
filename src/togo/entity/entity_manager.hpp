#line 2 "togo/entity/entity_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief EntityManager interface.
@ingroup entity
@ingroup entity_manager
*/

#pragma once

#include <togo/config.hpp>
#include <togo/entity/types.hpp>

namespace togo {
namespace entity_manager {

/**
	@addtogroup entity_manager
	@{
*/

/// Check if an entity is alive.
bool alive(
	EntityManager const& em,
	EntityID const& id
);

/// Create an entity.
EntityID create(EntityManager& em);

/// Destroy an entity.
void destroy(
	EntityManager& em,
	EntityID const& id
);

/// Shutdown.
///
/// Removes all entities.
/// This should only be used as part of system deinitialization.
void shutdown(EntityManager& em);

/** @} */ // end of doc-group entity_manager

} // namespace entity_manager
} // namespace togo
