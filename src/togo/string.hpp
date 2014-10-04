#line 2 "togo/string.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file string.hpp
@brief String interface.
@ingroup string
*/

#pragma once

#include <togo/config.hpp>
#include <togo/tags.hpp>
#include <togo/string_types.hpp>
#include <togo/fixed_array.hpp>

#include <cstring>

namespace togo {

namespace string {

/**
	@addtogroup string
	@{
*/

/// Get size of a NUL-terminated string.
///
/// This does not include the NUL terminator.
inline unsigned size(char const* cstr) {
	return cstr ? std::strlen(cstr) : 0;
}

/// Get size of a string literal or string array.
///
/// This does not include the NUL terminator if there is one.
/// If there are NULs before N, they are counted.
template<unsigned N>
inline constexpr unsigned size(char const (&data)[N]) {
	return N > 0 && data[N - 1] == '\0' ? N - 1 : N;
}

/// Get size of a fixed-capacity string.
///
/// This does not include the NUL terminator if there is one.
template<unsigned N>
inline unsigned size(FixedArray<char, N> const& string) {
	return
		!fixed_array::empty(string) && fixed_array::back(string) == '\0'
		? fixed_array::size(string) - 1
		: fixed_array::size(string)
	;
}

/// Compare two strings for equality.
///
/// This will short-circuit if lhs and rhs are not the same size.
bool compare_equal(StringRef const& lhs, StringRef const& rhs);

/// Copy string.
///
/// dst will be NUL-terminated.
/// dst must be of size src.size + 1.
void copy(char* dst, StringRef const& src);

/// Copy string.
///
/// The data of dst is overwritten.
/// The capacity of dst (N) must be at least src.size + 1.
template<unsigned N>
inline void copy(
	FixedArray<char, N>& dst,
	StringRef const& src
) {
	fixed_array::resize(dst, src.size + 1);
	copy(fixed_array::begin(dst), src);
}

/// Trim trailing slashes from string.
///
/// The first trailing slash is replaced by a NUL.
/// Returns new size of string (not including NUL terminator).
unsigned trim_trailing_slashes(char* string, unsigned size);

/// Trim trailing slashes from string.
///
/// Returns new size of string (not including NUL terminator).
template<unsigned N>
inline unsigned trim_trailing_slashes(FixedArray<char, N>& string) {
	unsigned const size = string::size(string);
	unsigned const new_size = trim_trailing_slashes(
		fixed_array::begin(string),
		size
	);
	if (new_size < size) {
		fixed_array::resize(string, new_size + 1);
	}
	return new_size;
}

/** @} */ // end of doc-group string

} // namespace string

/// Construct to null/empty.
inline StringRef::StringRef(null_tag)
	: data(nullptr)
	, size(0)
{}

/// Construct to NUL-terminated string.
inline StringRef::StringRef(char const* const cstr, cstr_tag)
	: data(cstr)
	, size(string::size(cstr))
{}

/// Construct to explicitly-sized data.
inline constexpr StringRef::StringRef(char const* const data, unsigned const size)
	: data(data)
	, size(
		data
		? (size > 0 && data[size - 1] == '\0' ? size - 1 : size)
		: 0
	)
{}

/// Construct to string literal or string array.
template<unsigned N>
inline constexpr StringRef::StringRef(char const (&data)[N])
	: StringRef(data, N)
{}

/// Construct to fixed-size character array.
template<unsigned N>
inline StringRef::StringRef(FixedArray<char, N> const& array)
	: StringRef(fixed_array::begin(array), fixed_array::size(array))
{}

/// Check if the reference has non-null data.
inline bool StringRef::valid() const {
	return data != nullptr;
}

/// Check if the reference points to non-empty string.
inline bool StringRef::any() const {
	return size > 0;
}

/// Check if the reference points to an empty string.
inline bool StringRef::empty() const {
	return size == 0;
}

} // namespace togo
