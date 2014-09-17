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

#include <cstring>

namespace togo {

/// Construct to null/empty.
inline StringRef::StringRef(null_tag)
	: data(nullptr)
	, size(0)
{}

/// Construct to NUL-terminated string.
inline StringRef::StringRef(char const* const cstr, cstr_tag)
	: data(cstr)
	, size(cstr ? std::strlen(cstr) : 0)
{}

/// Construct to explicitly-sized data.
inline StringRef::StringRef(char const* const data, unsigned const size)
	: data(data)
	, size(
		data
		? (size > 0 && data[size - 1] == '\0' ? size - 1 : size)
		: 0
	)
{}

/// Construct to string literal or string array.
template<unsigned N>
inline StringRef::StringRef(char const (&data)[N])
	: StringRef(data, N)
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

namespace string {

/**
	@addtogroup string
	@{
*/
/** @} */ // end of doc-group string

} // namespace string

} // namespace togo
