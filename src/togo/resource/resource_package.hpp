#line 2 "togo/resource/resource_package.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourcePackage interface.
@ingroup resource
@ingroup resource_package
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource_package.gen_interface>

namespace togo {
namespace resource_package {

/**
	@addtogroup resource_package
	@{
*/

/// Get name hash (cached).
inline ResourcePackageNameHash name_hash(ResourcePackage const& pkg) {
	return pkg._name_hash;
}

/// Get name.
inline StringRef name(ResourcePackage const& pkg) {
	return pkg._name;
}

/// Get path.
inline StringRef path(ResourcePackage const& pkg) {
	return pkg._path;
}

/** @} */ // end of doc-group resource_package

} // namespace resource_package
} // namespace togo
