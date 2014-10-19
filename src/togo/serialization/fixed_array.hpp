#line 2 "togo/serialization/fixed_array.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief FixedArray serialization.
@ingroup serialization
*/

#pragma once

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/serialization/support.hpp>
#include <togo/fixed_array.hpp>

#include <limits>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

namespace {
	template<class> struct ser_match_inner : false_type {};
	template<class T, unsigned N>
	struct ser_match_inner<FixedArray<T, N>> : true_type {};
} // anonymous namespace

template<class Ser, class S, class T, unsigned N>
inline void
read(serializer_tag, Ser& ser, SerCollection<S, FixedArray<T, N>>&& value) {
	static_assert(
		std::numeric_limits<S>::max() >= N,
		"S is smaller than the fixed capacity of this collection"
	);

	auto& collection = value.ref;
	S size{};
	ser % size;

	// NB: Assertion in resize() will cover invalid sizes
	fixed_array::resize(collection, size);
	ser % make_ser_sequence(
		fixed_array::begin(collection),
		fixed_array::size(collection)
	);
}

template<class Ser, class S, class T, unsigned N, bool C>
inline void
write(serializer_tag, Ser& ser, SerCollection<S, FixedArray<T, N>, C> const& value) {
	static_assert(
		std::numeric_limits<S>::max() >= N,
		"S is smaller than the fixed capacity of this collection"
	);

	auto const& collection = value.ref;
	auto const size = fixed_array::size(collection);
	TOGO_DEBUG_ASSERTE(std::numeric_limits<S>::max() >= size);
	ser
		% static_cast<S>(size)
		% make_ser_sequence(
			fixed_array::begin(collection),
			size
		)
	;
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serialization

} // namespace togo
