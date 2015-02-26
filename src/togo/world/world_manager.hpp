#line 2 "togo/world/world_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief WorldManager interface.
@ingroup world
@ingroup world_manager
*/

#pragma once

#include <togo/config.hpp>
#include <togo/world/types.hpp>

namespace togo {
namespace world_manager {

/**
	@addtogroup world_manager
	@{
*/

/// Check if a world is alive.
bool alive(
	WorldManager const& wm,
	WorldID const& id
);

/// Register a component manager.
///
/// An assertion will fail if the component name has already been
/// registered.
/// Component managers must be registered before any worlds are created.
void register_component_manager(
	WorldManager& wm,
	ComponentManagerDef const& def
);

/// Create a world.
WorldID create(WorldManager& wm);

/// Destroy a world.
void destroy(
	WorldManager& wm,
	WorldID const& id
);

/// Shutdown.
///
/// Removes all worlds and component manager definitions.
/// This should only be used as part of system deinitialization.
/// Using it during runtime can lead to zombie IDs pointing to valid
/// worlds (i.e., not the world originally created with the ID).
void shutdown(WorldManager& wm);

/** @} */ // end of doc-group world_manager

} // namespace world_manager
} // namespace togo
