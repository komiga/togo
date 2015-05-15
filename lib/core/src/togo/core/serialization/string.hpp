#line 2 "togo/core/serialization/string.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief String serialization.
@ingroup lib_core_serialization
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/serialization/support.hpp>

#include <limits>

namespace togo {

/**
	@addtogroup lib_core_serialization
	@{
*/

/** @cond INTERNAL */

// StringRef

template<class Ser, class S, bool C>
inline void
write(serializer_tag, Ser& ser, SerString<S, StringRef, C> const& value) {
	auto const& str = value.ref;
	TOGO_DEBUG_ASSERTE(std::numeric_limits<S>::max() >= str.size);
	ser
		% static_cast<S>(str.size)
		% make_ser_buffer(str.data, str.size)
	;
}

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
	auto const size = string::size(str);
	TOGO_DEBUG_ASSERTE(std::numeric_limits<S>::max() >= size);
	ser
		% static_cast<S>(size)
		% make_ser_buffer(str, size)
	;
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group lib_core_serialization

} // namespace togo
