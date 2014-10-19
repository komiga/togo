#line 2 "togo/serialization/array.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Array serialization.
@ingroup serialization
*/

#pragma once

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/serialization/support.hpp>
#include <togo/array.hpp>

#include <limits>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

template<class Ser, class S, class T>
inline void
read(serializer_tag, Ser& ser, SerCollection<S, Array<T>>&& value) {
	auto& collection = value.ref;
	S size{};
	ser % size;

	array::resize(collection, size);
	ser % make_ser_sequence(
		array::begin(collection),
		array::size(collection)
	);
}

template<class Ser, class S, class T, bool C>
inline void
write(serializer_tag, Ser& ser, SerCollection<S, Array<T>, C> const& value) {
	auto const& collection = value.ref;
	auto const size = array::size(collection);
	TOGO_DEBUG_ASSERTE(std::numeric_limits<S>::max() >= size);
	ser
		% static_cast<S>(size)
		% make_ser_sequence(
			array::begin(collection),
			size
		)
	;
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serialization

} // namespace togo
