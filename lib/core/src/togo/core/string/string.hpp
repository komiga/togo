#line 2 "togo/core/string/string.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief String interface.
@ingroup lib_core_string
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/utility/tags.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/string/string.gen_interface>

#include <cstring>

namespace togo {

namespace string {

/**
	@addtogroup lib_core_string
	@{
*/

/// Calculate size of a NUL-terminated string.
///
/// This does not include the NUL terminator.
inline unsigned size(char const* cstr) {
	return cstr ? std::strlen(cstr) : 0;
}

/// Calculate size of a string literal.
///
/// This does not include the NUL terminator if there is one.
/// If there are NULs before N, they are counted.
///
/// @warning This should only be called for string literals.
/// The C-string variant should be used for character arrays that may
/// have NULs before N.
template<unsigned N>
inline constexpr unsigned size_literal(char const (&data)[N]) {
	return N > 0 && data[N - 1] == '\0' ? N - 1 : N;
}

/// Calculate size of a fixed-capacity string.
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

/// Calculate size of a string.
///
/// This does not include the NUL terminator if there is one.
inline unsigned size(Array<char> const& string) {
	return
		!array::empty(string) && array::back(string) == '\0'
		? array::size(string) - 1
		: array::size(string)
	;
}

/// Copy string.
///
/// dst will be NUL-terminated.
/// The capacity of dst (N) must be at least src.size + 1.
template<unsigned N>
inline void copy(char (&dst)[N], StringRef const& src) {
	copy(dst, N, src);
}

/// Copy string.
///
/// dst will be NUL-terminated.
/// The capacity of dst (N) must be at least src.size + 1.
template<unsigned N>
inline void copy(
	FixedArray<char, N>& dst,
	StringRef const& src
) {
	fixed_array::resize(dst, src.size + 1);
	copy(fixed_array::begin(dst), fixed_array::size(dst), src);
}

/// Copy string.
///
/// dst will be NUL-terminated.
inline void copy(
	Array<char>& dst,
	StringRef const& src
) {
	array::resize(dst, src.size + 1);
	copy(array::begin(dst), array::size(dst), src);
}

/// Append to string.
///
/// dst will be NUL-terminated.
/// The capacity of dst must be at least size + str.size + 1.
inline void append(
	char* const dst,
	unsigned const capacity,
	unsigned const size,
	StringRef const& str
) {
	copy(dst + size, capacity, str);
}

/// Append to string.
///
/// dst will be NUL-terminated.
/// The capacity of dst (N) must be at least size + str.size + 1.
template<unsigned N>
inline void append(char (&dst)[N], unsigned const size, StringRef const& str) {
	copy(dst + size, N, str);
}

/// Append to string.
///
/// dst will be NUL-terminated.
/// The capacity of dst (N) must be at least
/// string::size(dst) + str.size + 1.
template<unsigned N>
inline void append(FixedArray<char, N>& dst, StringRef const& str) {
	unsigned const size = string::size(dst);
	fixed_array::resize(dst, size + str.size + 1);
	copy(fixed_array::begin(dst) + size, str.size + 1, str);
}

/// Append to string.
///
/// dst will be NUL-terminated.
inline void append(Array<char>& dst, StringRef const& str) {
	unsigned const size = string::size(dst);
	array::resize(dst, size + str.size + 1);
	copy(array::begin(dst) + size, str.size + 1, str);
}

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

/// Ensure string has a trailing slash by appending if necessary.
///
/// Returns new size of the string (not including NUL terminator).
template<unsigned N>
inline unsigned ensure_trailing_slash(FixedArray<char, N>& string) {
	unsigned const size = string::size(string);
	unsigned const new_size = ensure_trailing_slash(
		fixed_array::begin(string),
		fixed_array::capacity(string),
		size
	);
	if (new_size < size) {
		fixed_array::resize(string, new_size + 1);
	}
	return new_size;
}

/** @} */ // end of doc-group lib_core_string

} // namespace string

/// Construct to null/empty.
inline StringRef::StringRef()
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

/// Construct to string literal.
///
/// @warning This should only be used for string literals.
/// The sized or C-string constructors should be used for character
/// arrays that may have empty space before N.
template<unsigned N>
inline constexpr StringRef::StringRef(char const (&data)[N])
	: StringRef(data, N)
{}

/// Construct to fixed-size character array.
template<unsigned N>
inline StringRef::StringRef(FixedArray<char, N> const& array)
	: StringRef(fixed_array::begin(array), fixed_array::size(array))
{}

/// Whether the reference has non-null data.
inline bool StringRef::valid() const {
	return data != nullptr;
}

/// Whether the reference points to a non-empty string.
inline bool StringRef::any() const {
	return size > 0;
}

/// Whether the reference points to an empty string.
inline bool StringRef::empty() const {
	return size == 0;
}

/** @cond INTERNAL */
inline char const* begin(StringRef const& str) { return str.data; }
inline char const* cbegin(StringRef const& str) { return str.data; }

inline char const* end(StringRef const& str) { return str.data + str.size; }
inline char const* cend(StringRef const& str) { return str.data + str.size; }
/** @endcond */ // INTERNAL

} // namespace togo
