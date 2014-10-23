#line 2 "togo/serialization/string.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief String serialization.
@ingroup serialization
*/

#pragma once

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/serialization/support.hpp>
#include <togo/string.hpp>

#include <limits>

namespace togo {

/**
	@addtogroup serialization
	@{
*/

/** @cond INTERNAL */

// FixedArray<char, N>

template<class Ser, class S, unsigned N>
inline void
read(serializer_tag, Ser& ser, SerString<S, FixedArray<char, N>>&& value) {
	static_assert(
		std::numeric_limits<S>::max() >= N - 1,
		"S is smaller than the fixed capacity of this string"
	);

	auto& str = value.ref;
	S size{};
	ser % size;

	// NB: Assertion in resize() will cover invalid sizes
	fixed_array::resize(str, size);
	ser % make_ser_buffer(
		fixed_array::begin(str),
		fixed_array::size(str)
	);
	fixed_array::push_back(str, '\0');
}

template<class Ser, class S, unsigned N, bool C>
inline void
write(serializer_tag, Ser& ser, SerString<S, FixedArray<char, N>, C> const& value) {
	static_assert(
		std::numeric_limits<S>::max() >= N - 1,
		"S is smaller than the fixed capacity of this string"
	);

	auto const& str = value.ref;
	auto const size = string::size(str);
	TOGO_DEBUG_ASSERTE(std::numeric_limits<S>::max() >= size);
	ser
		% static_cast<S>(size)
		% make_ser_buffer(
			fixed_array::begin(str),
			size
		)
	;
}

// char[N]

template<class Ser, class S, unsigned N>
inline void
read(serializer_tag, Ser& ser, SerString<S, char[N]>&& value) {
	static_assert(
		std::numeric_limits<S>::max() >= N - 1,
		"S is smaller than the fixed capacity of this string"
	);

	auto& str = value.ref;
	S size{};
	ser % size;

	// NB: Assertion in resize() will cover invalid sizes
	TOGO_ASSERTE(size < N);
	ser % make_ser_buffer(str, size);
	str[size] = '\0';
}

template<class Ser, class S, unsigned N, bool C>
inline void
write(serializer_tag, Ser& ser, SerString<S, char[N], C> const& value) {
	static_assert(
		std::numeric_limits<S>::max() >= N - 1,
		"S is smaller than the fixed capacity of this string"
	);

	auto const& str = value.ref;
	auto const size = string::size(str, cstr_tag{});
	TOGO_DEBUG_ASSERTE(std::numeric_limits<S>::max() >= size);
	ser
		% static_cast<S>(size)
		% make_ser_buffer(str, size)
	;
}

/** @} */ // end of doc-group serialization

} // namespace togo
