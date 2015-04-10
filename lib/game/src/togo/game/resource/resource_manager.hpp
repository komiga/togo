#line 2 "togo/game/resource/resource_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceManager interface.
@ingroup lib_game_resource
@ingroup lib_game_resource_manager

@defgroup lib_game_resource_manager ResourceManager
@ingroup lib_game_resource
@details
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/string/types.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource_manager.gen_interface>

namespace togo {
namespace game {
namespace resource_manager {

/**
	@addtogroup lib_game_resource_manager
	@{
*/

/// Base path.
inline StringRef base_path(
	ResourceManager const& rm
) {
	return rm._base_path;
}

/// Package collection.
inline Array<ResourcePackage*> const& packages(
	ResourceManager const& rm
) {
	return rm._packages;
}

/** @} */ // end of doc-group lib_game_resource_manager

} // namespace resource_manager
} // namespace game
} // namespace togo
