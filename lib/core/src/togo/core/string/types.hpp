#line 2 "togo/core/string/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief String types.
@ingroup lib_core_types
@ingroup lib_core_string
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/utility/tags.hpp>

namespace togo {

// Forward declarations
template<class T, unsigned N>
struct FixedArray; // external

/**
	@addtogroup lib_core_string
	@{
*/

/// Constant string reference.
struct StringRef {
	/// String data.
	char const* data;
	/// String size (not including terminating NUL if there is one).
	unsigned size;


	StringRef(StringRef const&) = default;
	StringRef& operator=(StringRef const&) = default;
	StringRef(StringRef&&) = default;
	StringRef& operator=(StringRef&&) = default;

	StringRef();
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

/** @} */ // end of doc-group lib_core_string

} // namespace togo
