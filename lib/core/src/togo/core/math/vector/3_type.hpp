#line 2 "togo/core/math/vector/3_type.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 3-dimensional vector.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_vector
@ingroup lib_core_math_vector_3d

@defgroup lib_core_math_vector_3d 3-dimensional vector
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

TOGO_DECLARE_TYPE_IS_VECTOR(TVec3);
/** @endcond */

/**
	@addtogroup lib_core_math_vector_3d
	@{
*/

/// Generic 3-dimensional vector.
template<class T>
struct TVec3 {
	static_assert(is_arithmetic<T>::value, "");

	/// Type of @c *this.
	using type = TVec3<T>;
	/// Type of components.
	using value_type = T;
	/// Size/length type.
	using size_type = unsigned;

	struct operations;

/** @name Fields */ /// @{
	value_type x; ///< X value.
	value_type y; ///< Y value.
	value_type z; ///< Z value.
/// @}

/** @name Constructors */ /// @{
	/// Construct zeroed.
	TVec3()
		: x{T(0)}, y{T(0)}, z{T(0)}
	{}

	/// Construct uninitialized.
	explicit TVec3(no_init_tag) {}

	/// Construct all components to value.
	explicit TVec3(value_type const& s)
		: x{s}, y{s}, z{s}
	{}

	/// Construct all components to value.
	template<class U>
	explicit TVec3(U const& s)
		: x{T(s)}, y{T(s)}, z{T(s)}
	{}

	/// Construct to values.
	explicit TVec3(
		value_type const& c1,
		value_type const& c2,
		value_type const& c3
	)
		: x{c1}, y{c2}, z{c3}
	{}

	/// Construct to values.
	template<class U, class V, class H>
	explicit TVec3(
		U const& c1,
		V const& c2,
		H const& c3
	)
		: x{T(c1)}, y{T(c2)}, z{T(c3)}
	{}

	/// Construct to vector.
	TVec3(type const& v) = default;

	/// Construct to vector.
	template<class U>
	TVec3(
		TVec3<U> const& v)
		: x{T(v.x)}, y{T(v.y)}, z{T(v.z)}
	{}

	/// Construct to X value and YZ vector.
	template<class U, class V>
	explicit TVec3(U const& c1, TVec2<V> const& v)
		: x{T(c1)}, y{T(v.x)}, z{T(v.y)}
	{}

	/// Construct to XY vector and Z value.
	template<class U, class V>
	explicit TVec3(TVec2<U> const& v, V const& c3)
		: x{T(v.x)}, y{T(v.y)}, z{T(c3)}
	{}

	/// Construct to higher-dimension vector.
	template<class U>
	explicit TVec3(TVec4<U> const& v)
		: x{T(v.x)}, y{T(v.y)}, z{T(v.z)}
	{}
/// @}

/** @name Properties */ /// @{
	/// Number of components.
	static constexpr size_type size() {
		return size_type(3);
	}

	/// Value at index.
	value_type& operator[](size_type const& i) {
		TOGO_DEBUG_ASSERTE(size() > i);
		return (&x)[i];
	}

	/// Value at index.
	value_type const& operator[](size_type const& i) const {
		TOGO_DEBUG_ASSERTE(size() > i);
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
		return *this;
	}

	/// Assign to vector.
	type& operator=(type const& v) = default;

	/// Assign to vector.
	template<class U>
	type& operator=(TVec3<U> const& v) {
		this->x = T(v.x);
		this->y = T(v.y);
		this->z = T(v.z);
		return *this;
	}
/// @}
};

/** @} */ // end of doc-group lib_core_math_vector_3d

} // namespace math
} // namespace togo
