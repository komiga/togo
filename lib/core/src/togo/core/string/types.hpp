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
#include <togo/core/utility/types.hpp>

namespace togo {

// Forward declarations
namespace fixed_array {
	template<class T, unsigned N>
	struct FixedArray; // external
} // namespace fixed_array

namespace array {
	template<class T>
	struct Array; // external
} // namespace array

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
	StringRef(StringRef&&) = default;
	StringRef& operator=(StringRef const&) = default;
	StringRef& operator=(StringRef&&) = default;

	StringRef();
	StringRef(char const* const cstr, cstr_tag);
	StringRef(char const* const data, char const* const end);
	constexpr StringRef(char const* const data, unsigned const size);
	template<unsigned N>
	constexpr StringRef(char const (&data)[N]);
	template<unsigned N>
	StringRef(fixed_array::FixedArray<char, N> const& array);
	StringRef(array::Array<char> const& array);

	bool valid() const;
	bool any() const;
	bool empty() const;

	char operator[](unsigned const i) const;
};

/** @} */ // end of doc-group lib_core_string

} // namespace togo
