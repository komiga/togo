#line 2 "togo/serialization/resource/resource_metadata.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Array serialization.
@ingroup serialization
*/

#pragma once

#include <togo/config.hpp>
#include <togo/serialization/support.hpp>
#include <togo/resource/types.hpp>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, ResourceMetadata& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.name_hash
		% value.tags_hash
		% value.type
		% value.data_format_version
		% value.data_offset
		% value.data_size
	;
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serialization

} // namespace togo
