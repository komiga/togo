#line 2 "togo/serialization/resource/test_resource.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief TestResource serialization.
@ingroup serialization
*/

#pragma once

#include <togo/config.hpp>
#include <togo/resource/types.hpp>
#include <togo/serialization/support.hpp>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

template<class Ser>
inline void
serialize(serializer_tag, Ser& ser, TestResource& value_unsafe) {
	auto& value = serializer_cast_safe<Ser>(value_unsafe);
	ser % value.x;
}

/** @} */ // end of doc-group serialization

} // namespace togo
