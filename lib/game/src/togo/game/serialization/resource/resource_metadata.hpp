#line 2 "togo/game/serialization/resource/resource_metadata.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceMetadata serialization.
@ingroup lib_game_serialization
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/game/resource/types.hpp>

namespace togo {
namespace game {

/**
	@addtogroup lib_game_serialization
	@{
*/

/** @cond INTERNAL */

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, ResourceMetadata& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser
		% value.name_hash
		% value.tag_glob_hash
		% value.type
		% value.data_format_version
		% value.data_offset
		% value.data_size
	;
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group lib_game_serialization

} // namespace game
} // namespace togo
