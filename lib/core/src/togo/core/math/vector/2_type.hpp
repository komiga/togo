#line 2 "togo/core/math/vector/2_type.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 2-dimensional vector.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_vector
@ingroup lib_core_math_vector_2d

@defgroup lib_core_math_vector_2d 2-dimensional vector
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
template<class T> struct TVec2;
template<class T> struct TVec3;
template<class T> struct TVec4;

TOGO_DECLARE_TYPE_IS_VECTOR(TVec2);
/** @endcond */

/**
	@addtogroup lib_core_math_vector_2d
	@{
*/

/// Generic 2-dimensional vector.
template<class T>
struct TVec2 {
	static_assert(is_arithmetic<T>::value, "");

	/// Type of @c *this.
	using type = TVec2<T>;
	/// Type of components.
	using value_type = T;
	/// Size/length type.
	using size_type = unsigned;

	struct operations;

/** @name Fields */ /// @{
	/// X value.
	union {
		value_type x;
		value_type width;
	};
	/// Y value.
	union {
		value_type y;
		value_type height;
	};
/// @}

/** @name Constructors */ /// @{
	/// Construct zeroed.
	TVec2()
		: x{T(0)}, y{T(0)}
	{}

	/// Construct uninitialized.
	explicit TVec2(no_init_tag) {}

	/// Construct all components to value.
	explicit TVec2(value_type const& s)
		: x{s}, y{s}
	{}

	/// Construct all components to value.
	template<class U>
	explicit TVec2(U const& s)
		: x{T(s)}, y{T(s)}
	{}

	/// Construct to values.
	explicit TVec2(value_type const& c1, value_type const& c2)
		: x{c1}, y{c2}
	{}

	/// Construct to values.
	template<class U, class V>
	explicit TVec2(U const& c1, V const& c2)
		: x{T(c1)}, y{T(c2)}
	{}

	/// Construct to vector.
	TVec2(type const& v) = default;

	/// Construct to vector.
	template<class U>
	TVec2(TVec2<U> const& v)
		: x{T(v.x)}, y{T(v.y)}
	{}
	
	/// Construct to slice of higher-dimension vector.
	template<class U>
	explicit TVec2(TVec3<U> const& v)
		: x{T(v.x)}, y{T(v.y)}
	{}

	/// Construct to slice of higher-dimension vector.
	template<class U>
	explicit TVec2(TVec4<U> const& v)
		: x{T(v.x)}, y{T(v.y)}
	{}
/// @}

/** @name Properties */ /// @{
	/// Number of components.
	static constexpr size_type size() {
		return size_type(2);
	}

	/// Value at index.
	value_type& operator[](size_type const& i) {
		TOGO_DEBUG_ASSERTE(i < size());
		return (&x)[i];
	}

	/// Value at index.
	value_type const& operator[](size_type const& i) const {
		TOGO_DEBUG_ASSERTE(i < size());
		return (&x)[i];
	}
/// @}

/** @name Assignment operators */ /// @{
	/// Assign all components to value.
	template<class U>
	type& operator=(U const& s) {
		this->x = T(s);
		this->y = T(s);
		return *this;
	}

	/// Assign to vector.
	type& operator=(type const& v) = default;

	/// Assign to vector.
	template<class U>
	type& operator=(TVec2<U> const& v) {
		this->x = T(v.x);
		this->y = T(v.y);
		return *this;
	}
/// @}
};

/** @} */ // end of doc-group lib_core_math_vector_2d

} // namespace math
} // namespace togo
