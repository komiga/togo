#line 2 "togo/core/math/vector/1_type.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 1-dimensional vector.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_vector
@ingroup lib_core_math_vector_1d

@defgroup lib_core_math_vector_1d 1-dimensional vector
@ingroup lib_core_math
@ingroup lib_core_math_vector
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>

namespace togo {
namespace math {

// Forward declarations
/** @cond INTERNAL */
template<class T> struct TVec1;
template<class T> struct TVec2;
template<class T> struct TVec3;
template<class T> struct TVec4;

TOGO_DECLARE_TYPE_IS_VECTOR(TVec1);
/** @endcond */

/**
	@addtogroup lib_core_math_vector_1d
	@{
*/

/// Generic 1-dimensional vector.
template<class T>
struct TVec1 {
	static_assert(is_arithmetic<T>::value, "");

	/// Type of @c *this.
	using type = TVec1<T>;
	/// Type of components.
	using value_type = T;
	/// Size/length type.
	using size_type = unsigned;

	struct operations;

/** @name Fields */ /// @{
	value_type x; ///< X value.
/// @}

/** @name Constructors */ /// @{
	/// Construct zeroed.
	TVec1()
		: x{T(0)}
	{}

	/// Construct uninitialized.
	explicit TVec1(no_init_tag) {}

	/// Construct to value.
	explicit TVec1(value_type const& c1)
		: x{c1}
	{}

	/// Construct to value.
	template<class U>
	explicit TVec1(U const& c1)
		: x{T(c1)}
	{}

	/// Construct to vector.
	TVec1(type const& v) = default;

	/// Construct to vector.
	template<class U>
	TVec1(TVec1<U> const& v)
		: x{T(v.x)}
	{}

	/// Construct to slice of higher-dimension vector.
	template<class U>
	explicit TVec1(TVec2<U> const& v)
		: x{T(v.x)}
	{}

	/// Construct to slice of higher-dimension vector.
	template<class U>
	explicit TVec1(TVec3<U> const& v)
		: x{T(v.x)}
	{}

	/// Construct to slice of higher-dimension vector.
	template<class U>
	explicit TVec1(TVec4<U> const& v)
		: x{T(v.x)}
	{}
/// @}

/** @name Properties */ /// @{
	/// Number of components.
	static constexpr size_type size() {
		return size_type(1);
	}

	/// Value at index.
	value_type& operator[](size_type const& i) {
		(void)i;
		TOGO_DEBUG_ASSERTE(i < size());
		return this->x;
	}

	/// Value at index.
	value_type const& operator[](size_type const& i) const {
		(void)i;
		TOGO_DEBUG_ASSERTE(i < size());
		return this->x;
	}
/// @}

/** @name Assignment operators */ /// @{
	/// Assign to value.
	template<class U>
	type& operator=(U const& s) {
		this->x = T(s);
		return *this;
	}

	/// Assign to vector.
	type& operator=(type const& v) = default;

	/// Assign to vector.
	template<class U>
	type& operator=(TVec1<U> const& v) {
		this->x = T(v.x);
		return *this;
	}
/// @}
};

/** @} */ // end of doc-group lib_core_math_vector_1d

} // namespace math
} // namespace togo
