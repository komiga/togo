#line 2 "togo/resource.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file resource.hpp
@brief Resource interface.
@ingroup resource
*/

#pragma once

#include <togo/config.hpp>
#include <togo/string_types.hpp>
#include <togo/hash.hpp>
#include <togo/resource_types.hpp>

namespace togo {
namespace resource {

/**
	@addtogroup resource
	@{
*/

/// Calculate hash of resource type.
inline ResourceType hash_type(StringRef const& type) {
	return hash::calc32(type);
}

/// Calculate hash of resource name.
inline ResourceNameHash hash_name(StringRef const& name) {
	return hash::calc64(name);
}

/// Parse resource path.
///
/// Returns false if the given path is malformed.
bool parse_path(
	StringRef const& path,
	ResourcePathParts& pp
);

/** @} */ // end of doc-group resource

} // namespace resource
} // namespace togo
