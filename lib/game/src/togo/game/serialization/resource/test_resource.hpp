#line 2 "togo/game/serialization/resource/test_resource.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief TestResource serialization.
@ingroup serialization
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/game/resource/types.hpp>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

enum : u32 {
	SER_FORMAT_VERSION_TEST_RESOURCE = 1,
};

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, TestResource& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser % value.x;
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serialization

} // namespace togo
