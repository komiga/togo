#line 2 "togo/core/utility/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Utility types.
@ingroup lib_core_utility
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_core_utility
	@{
*/

/** @name Variation/placeholder tags */ /// @{

/// No-initialize constructor tag.
enum class no_init_tag {};

/// Null value tag.
enum class null_tag {};

/// Null reference tag.
enum class null_ref_tag {};

/// NUL-terminated string tag.
enum class cstr_tag {};

/// Boolean value tag.
enum class bool_tag {};

/// @}

/// Endianness.
enum class Endian : unsigned {
	/// Little endian.
	little = TOGO_ENDIAN_LITTLE,
	/// Big endian.
	big = TOGO_ENDIAN_BIG,
	/// System endian.
	system = TOGO_ENDIAN_SYSTEM,
};

/// Array reference.
template<class T>
struct ArrayRef {
	T* _begin;
	T* _end;

	ArrayRef(null_ref_tag const)
		: _begin(nullptr)
		, _end(nullptr)
	{}

	ArrayRef(T* const begin, T* const end)
		: _begin(begin)
		, _end(end)
	{}

	ArrayRef(T* const data, unsigned const size)
		: ArrayRef(data, data + size)
	{}

	template<unsigned N>
	ArrayRef(T (&data)[N])
		: ArrayRef(data, data + N)
	{}

	template<class U/*, class = enable_if<std::is_convertible<U, T>::value>*/>
	ArrayRef(ArrayRef<U> const& other)
		: _begin(other._begin)
		, _end(other._end)
	{}

	/// Number of items.
	unsigned size() const {
		return _end - _begin;
	}

	/// Get value by index.
	T& operator[](unsigned const i);

	/// Get value by index.
	T const& operator[](unsigned const i) const;
};

/** @} */ // end of doc-group lib_core_utility

} // namespace togo
