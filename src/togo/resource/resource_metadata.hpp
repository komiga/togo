#line 2 "togo/resource/resource_metadata.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceMetadata interface.
@ingroup resource_metadata
*/

#pragma once

#include <togo/config.hpp>
#include <togo/resource/types.hpp>

namespace togo {

// Forward declarations
template<class T, unsigned N>
struct FixedArray;

namespace resource_metadata {

/**
	@addtogroup resource_metadata
	@{
*/

/// Get compiled path.
void compiled_path(
	ResourceMetadata const& metadata,
	FixedArray<char, 24>& str
);

/** @} */ // end of doc-group resource_metadata

} // namespace resource_metadata

} // namespace togo
