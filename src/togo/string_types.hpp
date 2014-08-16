#line 2 "togo/string_types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file string_types.hpp
@brief String types.
@ingroup types
@ingroup string
*/

#pragma once

#include <togo/config.hpp>
#include <togo/tags.hpp>

namespace togo {

/**
	@addtogroup string
	@{
*/

/**
	Constant string reference.
*/
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

	StringRef(char const* const cstr, cstr_tag);
	StringRef(char const* const data, unsigned const size);
	template<unsigned N>
	StringRef(char const (&data)[N]);

	bool valid() const;
	bool any() const;
	bool empty() const;
};

/** @} */ // end of doc-group string

} // namespace togo
