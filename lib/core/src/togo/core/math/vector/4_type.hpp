#line 2 "togo/core/math/vector/4_type.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 4-dimensional vector.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_vector
@ingroup lib_core_math_vector_4d

@defgroup lib_core_math_vector_4d 4-dimensional vector
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

TOGO_DECLARE_TYPE_IS_VECTOR(TVec4);
/** @endcond */

/**
	@addtogroup lib_core_math_vector_4d
	@{
*/

/// Generic 4-dimensional vector.
template<class T>
struct TVec4 {
	static_assert(is_arithmetic<T>::value, "");

	/// Type of @c *this.
	using type = TVec4<T>;
	/// Type of components.
	using value_type = T;
	/// Size/length type.
	using size_type = unsigned;

	struct operations;

/** @name Fields */ /// @{
	/// X value.
	union {
		value_type x;
		value_type x1;
	};
	/// Y value.
	union {
		value_type y;
		value_type y1;
	};
	/// Z value.
	union {
		value_type z;
		value_type x2;
		value_type width;
	};
	/// W value.
	union {
		value_type w;
		value_type y2;
		value_type height;
	};
/// @}

/** @name Constructors */ /// @{
	/// Construct zeroed.
	TVec4()
		: x{T(0)}, y{T(0)}, z{T(0)}, w{T(0)}
	{}

	/// Construct uninitialized.
	explicit TVec4(no_init_tag) {}

	/// Construct all components to value.
	explicit TVec4(value_type const& s)
		: x{s}, y{s}, z{s}, w{s}
	{}

	/// Construct all components to value.
	template<class U>
	explicit TVec4(U const& s)
		: x{T(s)}, y{T(s)}, z{T(s)}, w{T(s)}
	{}

	/// Construct to values.
	explicit TVec4(
		value_type const& c1, value_type const& c2,
		value_type const& c3, value_type const& c4
	)
		: x{c1}, y{c2}, z{c3}, w{c4}
	{}
	/// Construct to values.
	template<class U, class V, class H, class L>
	explicit TVec4(
		U const& c1, V const& c2,
		H const& c3, L const& c4
	)
		: x{T(c1)}, y{T(c2)}, z{T(c3)}, w{T(c4)}
	{}

	/// Construct to vector.
	TVec4(type const& v) = default;

	/// Construct to vector.
	template<class U>
	TVec4(TVec4<U> const& v)
		: x{T(v.x)}, y{T(v.y)}, z{T(v.z)}, w{T(v.w)}
	{}


	/// Construct to X value and YZW vector.
	template<class U, class V>
	explicit TVec4(U const& c1, TVec3<V> const& v)
		: x{T(c1)}, y{T(v.x)}, z{T(v.y)}, w{T(v.z)}
	{}

	/// Construct to XYZ vector and W value.
	template<class U, class V>
	explicit TVec4(TVec3<U> const& v, V const& c4)
		: x{T(v.x)}, y{T(v.y)}, z{T(v.z)}, w{T(c4)}
	{}


	/// Construct to X value, Y value, and ZW vector.
	template<class U, class V, class H>
	explicit TVec4(
		U const& c1,
		V const& c2,
		TVec2<H> const& v
	)
		: x{T(c1)}, y{T(c2)}, z{T(v.x)}, w{T(v.y)}
	{}

	/// Construct to XY vector, Z value, and W value.
	template<class U, class V, class H>
	explicit TVec4(
		TVec2<U> const& v,
		V const& c3,
		H const& c4
	)
		: x{T(v.x)}, y{T(v.y)}, z{T(c3)}, w{T(c4)}
	{}

	/// Construct to XY and ZW vectors.
	template<class U, class V>
	explicit TVec4(TVec2<U> const& v1, TVec2<V> const& v2)
		: x{T(v1.x)}, y{T(v1.y)}, z{T(v2.x)}, w{T(v2.y)}
	{}
/// @}

/** @name Properties */ /// @{
	/// Number of components.
	static constexpr size_type size() {
		return size_type(4);
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
		this->z = T(s);
		this->w = T(s);
		return *this;
	}

	/// Assign to vector.
	type& operator=(type const& v) = default;

	/// Assign to vector.
	template<class U>
	type& operator=(TVec4<U> const& v) {
		this->x = T(v.x);
		this->y = T(v.y);
		this->z = T(v.z);
		this->w = T(v.w);
		return *this;
	}
/// @}
};

/** @} */ // end of doc-group lib_core_math_vector_4d

} // namespace math
} // namespace togo
