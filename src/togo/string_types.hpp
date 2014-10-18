#line 2 "togo/string_types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief String types.
@ingroup types
@ingroup string
*/

#pragma once

#include <togo/config.hpp>
#include <togo/tags.hpp>

namespace togo {

// Forward declarations
template<class T, unsigned N>
struct FixedArray;

/**
	@addtogroup string
	@{
*/

/// Constant string reference.
struct StringRef {
	/// String data.
	char const* data;
	/// String size (not including terminating NUL if there is one).
	unsigned size;

	StringRef() = delete;

	StringRef(StringRef const&) = default;
	StringRef& operator=(StringRef const&) = default;
	StringRef(StringRef&&) = default;
	StringRef& operator=(StringRef&&) = default;

	StringRef(null_tag);
	StringRef(char const* const cstr, cstr_tag);
	constexpr StringRef(char const* const data, unsigned const size);
	template<unsigned N>
	constexpr StringRef(char const (&data)[N]);
	template<unsigned N>
	StringRef(FixedArray<char, N> const& array);

	bool valid() const;
	bool any() const;
	bool empty() const;
};

/** @} */ // end of doc-group string

} // namespace togo
