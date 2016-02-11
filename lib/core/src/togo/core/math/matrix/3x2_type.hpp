#line 2 "togo/core/math/vector/3x2_type.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 3-by-2 matrix.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_matrix
@ingroup lib_core_math_matrix_3x2

@defgroup lib_core_math_matrix_3x2 3-by-2 matrix
@ingroup lib_core_math
@ingroup lib_core_math_matrix
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/vector/2_type.hpp>

namespace togo {
namespace math {

// Forward declarations
/** @cond INTERNAL */
template<class T> struct TVec2;
template<class T> struct TVec3;
template<class T> struct TMat2x2;
template<class T> struct TMat2x3;
template<class T> struct TMat3x2;
template<class T> struct TMat3x3;
template<class T> struct TMat4x2;
template<class T> struct TMat4x3;

TOGO_DECLARE_TYPE_IS_MATRIX(TMat3x2);
/** @endcond */

/**
	@addtogroup lib_core_math_matrix_3x2
	@{
*/

/// Generic 3-by-2 matrix.
template<class T>
struct TMat3x2 {
	static_assert(togo::is_floating_point<T>::value, "");

	/// Type of @c *this.
	using type = TMat3x2<T>;
	/// Type of transpose.
	using transpose_type = TMat2x3<T>;
	/// Type of components.
	using value_type = T;
	/// Type of rows.
	using row_type = TVec3<T>;
	/// Type of columns.
	using col_type = TVec2<T>;
	/// Size/length type.
	using size_type = unsigned;

	struct operations;

	/// Raw columns.
	col_type data[3];

/** @name Constructors */ /// @{
	/// Construct to identity.
	TMat3x2() : data{
		col_type{T(1), T(0)},
		col_type{T(0), T(1)},
		col_type{T(0), T(0)}
	} {}

	/// Construct uninitialized.
	explicit TMat3x2(no_init_tag) {}

	/// Construct to main diagonal.
	explicit TMat3x2(value_type const& s) : data{
		col_type{s, T(0)},
		col_type{T(0), s},
		col_type{T(0), T(0)}
	} {}

	/// Construct to main diagonal.
	template<class U>
	explicit TMat3x2(U const& s) : data{
		col_type{T(s), T(0)},
		col_type{T(0), T(s)},
		col_type{T(0), T(0)}
	} {}

	/// Construct to values.
	explicit TMat3x2(
		value_type const& x1, value_type const& y1,
		value_type const& x2, value_type const& y2,
		value_type const& x3, value_type const& y3
	) : data{
		col_type{x1, y1},
		col_type{x2, y2},
		col_type{x3, y3}
	} {}

	/// Construct to values.
	template<
		class X1, class Y1,
		class X2, class Y2,
		class X3, class Y3
	>
	explicit TMat3x2(
		X1 const& x1, Y1 const& y1,
		X2 const& x2, Y2 const& y2,
		X3 const& x3, Y3 const& y3
	) : data{
		col_type{T(x1), T(y1)},
		col_type{T(x2), T(y2)},
		col_type{T(x3), T(y3)}
	} {}

	/// Construct to column vectors.
	explicit TMat3x2(
		col_type const& c1,
		col_type const& c2,
		col_type const& c3
	) : data{
		c1,
		c2,
		c3
	} {}

	/// Construct to column vectors.
	template<class C1, class C2, class C3>
	explicit TMat3x2(
		TVec2<C1> const& c1,
		TVec2<C2> const& c2,
		TVec2<C3> const& c3
	) : data{
		col_type{c1},
		col_type{c2},
		col_type{c3}
	} {}

	/// Construct to matrix.
	TMat3x2(type const& m) = default;

	/// Construct to matrix.
	template<class U>
	TMat3x2(TMat3x2<U> const& m) : data{
		col_type{m.data[0]},
		col_type{m.data[1]},
		col_type{m.data[2]}
	} {}
/// @}

/** @name Properties */ /// @{
	/// Number of columns.
	static constexpr size_type size() {
		return size_type(3);
	}

	/// Number of components in column.
	static constexpr size_type col_size() {
		return col_type::size();
	}

	/// Number of components in row.
	static constexpr size_type row_size() {
		return row_type::size();
	}

	/// Column at index.
	col_type& operator[](size_type const& i) {
		TOGO_DEBUG_ASSERTE(size() > i);
		return data[i];
	}

	/// Column at index.
	col_type const& operator[](size_type const& i) const {
		TOGO_DEBUG_ASSERTE(size() > i);
		return data[i];
	}
/// @}

/** @name Assignment operators */ /// @{
	/// Assign to matrix.
	type& operator=(type const& m) = default;

	/// Assign to matrix.
	template<class U>
	type& operator=(TMat3x2<U> const& m) {
		data[0] = m.data[0];
		data[1] = m.data[1];
		data[2] = m.data[2];
		return *this;
	}
/// @}
};

/** @} */ // end of doc-group lib_core_math_matrix_3x2

} // namespace math
} // namespace togo
