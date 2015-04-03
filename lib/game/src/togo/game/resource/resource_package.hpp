#line 2 "togo/game/resource/resource_package.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourcePackage interface.
@ingroup resource
@ingroup resource_package
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/types.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource_package.gen_interface>

namespace togo {
namespace resource_package {

/**
	@addtogroup resource_package
	@{
*/

/// Name hash.
inline ResourcePackageNameHash name_hash(ResourcePackage const& pkg) {
	return pkg._name_hash;
}

/// Name.
inline StringRef name(ResourcePackage const& pkg) {
	return pkg._name;
}

/// Path.
inline StringRef path(ResourcePackage const& pkg) {
	return pkg._path;
}

/** @} */ // end of doc-group resource_package

} // namespace resource_package
} // namespace togo
