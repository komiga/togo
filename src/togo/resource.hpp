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
#include <togo/resource_types.hpp>

namespace togo {
namespace resource {

/**
	@addtogroup resource
	@{
*/

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
