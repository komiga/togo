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
#include <togo/resource/resource_manager.gen_interface>

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

/** @} */ // end of doc-group resource_manager

} // namespace resource_manager
} // namespace togo
