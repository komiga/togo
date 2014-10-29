#line 2 "togo/tool_build/resource_metadata.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceMetadata interface.
@ingroup tool_build_resource_metadata
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>

namespace togo {

// Forward declarations
template<class T, unsigned N>
struct FixedArray;

namespace tool_build {
namespace resource_metadata {

/**
	@addtogroup resource_metadata
	@{
*/

/// Get output path.
void output_path(
	ResourceMetadata const& metadata,
	FixedArray<char, 24>& str
);

/** @} */ // end of doc-group resource_metadata

} // namespace resource_metadata
} // namespace tool_build

} // namespace togo
