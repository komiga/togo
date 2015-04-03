#line 2 "togo/game/resource/resource_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceManager interface.
@ingroup resource
@ingroup resource_manager
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/string/types.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource_manager.gen_interface>

namespace togo {
namespace resource_manager {

/**
	@addtogroup resource_manager
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

/** @} */ // end of doc-group resource_manager

} // namespace resource_manager
} // namespace togo
