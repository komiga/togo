#line 2 "togo/core/math/vector/2x2.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 2-by-2 matrix interface.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_matrix
@ingroup lib_core_math_matrix_2x2
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/matrix/2x2_type.hpp>

#include <cmath>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_matrix_2x2
	@{
*/

/** @cond INTERNAL */
template<class T>
struct TMat2x2<T>::operations {
	using type = typename TMat2x2<T>::type;
	using type_cref = type const&;
	using value_type = typename type::value_type;
	using value_cref = typename type::value_type const&;
	using row_type = typename type::row_type;
	using col_type = typename type::col_type;
	using row_cref = row_type const&;
	using col_cref = col_type const&;
	using transpose_type = typename type::transpose_type;

	static transpose_type transpose(type_cref m) {
		return transpose_type{
			m.data[0].x, m.data[1].x,
			m.data[0].y, m.data[1].y
		};
	}

	static value_type determinant(type_cref m) {
		return
			m.data[0].x * m.data[1].y -
			m.data[1].x * m.data[0].y
		;
	}

	static type inverse(type_cref m) {
		value_type const det = operations::determinant(m);
		return type{
			 m.data[1].y / det, -m.data[0].y / det,
			-m.data[1].x / det,  m.data[0].x / det
		};
	}

	static type unary_negative(type_cref m) {
		return type{
			-m.data[0],
			-m.data[1]
		};
	}

	static type scalar_add(type_cref m, value_cref s) {
		return type{
			m.data[0] + s,
			m.data[1] + s
		};
	}

	static type add(type_cref m, type_cref n) {
		return type{
			m.data[0] + n.data[0],
			m.data[1] + n.data[1]
		};
	}

	static type scalar_subtract_rhs(type_cref m, value_cref s) {
		return type{
			m.data[0] - s,
			m.data[1] - s
		};
	}

	static type scalar_subtract_lhs(type_cref m, value_cref s) {
		return type{
			s - m.data[0],
			s - m.data[1]
		};
	}

	static type subtract(type_cref m, type_cref n) {
		return type{
			m.data[0] - n.data[0],
			m.data[1] - n.data[1]
		};
	}

	static type scalar_multiply(type_cref m, value_cref s) {
		return type{
			m.data[0] * s,
			m.data[1] * s
		};
	}

	static col_type row_multiply(type_cref m, row_cref v) {
		return col_type{
			m.data[0].x * v.x + m.data[1].x * v.y,
			m.data[0].y * v.x + m.data[1].y * v.y
		};
	}

	static row_type col_multiply(type_cref m, col_cref v) {
		return row_type{
			m.data[0].x * v.x + m.data[0].y * v.y,
			m.data[1].x * v.x + m.data[1].y * v.y
		};
	}

	static type multiply(type_cref m, type_cref n) {
		return type{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y
		};
	}

	static TMat3x2<T> multiply(type_cref m, TMat3x2<T> const& n) {
		return TMat3x2<T>{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y,
			m.data[0].x * n.data[2].x + m.data[1].x * n.data[2].y,
			m.data[0].y * n.data[2].x + m.data[1].y * n.data[2].y
		};
	}

	static TMat4x2<T> multiply(type_cref m, TMat4x2<T> const& n) {
		return TMat4x2<T>{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y,
			m.data[0].x * n.data[2].x + m.data[1].x * n.data[2].y,
			m.data[0].y * n.data[2].x + m.data[1].y * n.data[2].y,
			m.data[0].x * n.data[3].x + m.data[1].x * n.data[3].y,
			m.data[0].y * n.data[3].x + m.data[1].y * n.data[3].y
		};
	}

	static type scalar_divide_rhs(type_cref m, value_cref s) {
		return type{
			m.data[0] / s,
			m.data[1] / s
		};
	}

	static type scalar_divide_lhs(type_cref m, value_cref s) {
		return type{
			s / m.data[0],
			s / m.data[1]
		};
	}

	static col_type row_divide(type_cref m, row_cref v) {
		return row_multiply(operations::inverse(m), v);
	}

	static row_type col_divide(type_cref m, col_cref v) {
		return col_multiply(operations::inverse(m), v);
	}

	static type divide(type_cref m, type_cref n) {
		return multiply(m, operations::inverse(n));
	}
};
/** @endcond */ // INTERNAL

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
/** @name Comparison operators */ /// @{
	/// Equivalence operator.
	template<class T>
	inline bool operator==(TMat2x2<T> const& m, TMat2x2<T> const& n) {
		return
			m.data[0] == n.data[0] &&
			m.data[1] == n.data[1]
		;
	}

	/// Non-equivalence operator.
	template<class T>
	inline bool operator!=(TMat2x2<T> const& m, TMat2x2<T> const& n) {
		return
			m.data[0] != n.data[0] ||
			m.data[1] != n.data[1]
		;
	}
/// @}
#pragma GCC diagnostic pop

/** @name Arithmetic assignment operators */ /// @{
	/// Add value to all components.
	template<class T, class U>
	inline TMat2x2<T>& operator+=(TMat2x2<T>& m, U const& s) {
		m.data[0] += T(s);
		m.data[1] += T(s);
		return m;
	}

	/// Add matrix.
	template<class T, class U>
	inline TMat2x2<T>& operator+=(TMat2x2<T>& m, TMat2x2<U> const& n) {
		m.data[0] += n.data[0];
		m.data[1] += n.data[1];
		return m;
	}

	/// Subtract value from all components.
	template<class T, class U>
	inline TMat2x2<T>& operator-=(TMat2x2<T>& m, U const& s) {
		m.data[0] -= T(s);
		m.data[1] -= T(s);
		return m;
	}

	/// Subtract matrix.
	template<class T, class U>
	inline TMat2x2<T>& operator-=(TMat2x2<T>& m, TMat2x2<U> const& n) {
		m.data[0] -= n.data[0];
		m.data[1] -= n.data[1];
		return m;
	}

	/// Multiply by scalar.
	template<class T, class U>
	inline TMat2x2<T>& operator*=(TMat2x2<T>& m, U const& s) {
		m.data[0] *= T(s);
		m.data[1] *= T(s);
		return m;
	}

	/// Multiply by matrix (proper product).
	template<class T, class U>
	inline TMat2x2<T>& operator*=(TMat2x2<T>& m, TMat2x2<U> const& n) {
		return (m = m * n);
	}

	/// Divide all components by value.
	template<class T, class U>
	inline TMat2x2<T>& operator/=(TMat2x2<T>& m, U const& s) {
		m.data[0] /= T(s);
		m.data[1] /= T(s);
		return m;
	}

	/// Divide by matrix (proper quotient).
	template<class T, class U>
	inline TMat2x2<T>& operator/=(TMat2x2<T>& m, TMat2x2<U> const& n) {
		return (m = m / n);
	}
/// @}

/** @name Increment and decrement operators */ /// @{
	/// Prefix increment.
	template<class T>
	inline TMat2x2<T>& operator++(TMat2x2<T>& m) {
		++m.data[0];
		++m.data[1];
		return m;
	}

	/// Prefix decrement.
	template<class T>
	inline TMat2x2<T>& operator--(TMat2x2<T>& m) {
		--m.data[0];
		--m.data[1];
		return m;
	}

	/// Matrix postfix increment.
	template<class T>
	inline TMat2x2<T> operator++(TMat2x2<T>& m, signed) {
		TMat2x2<T> c{m};
		++m;
		return c;
	}

	/// Matrix postfix decrement.
	template<class T>
	inline TMat2x2<T> operator--(TMat2x2<T>& m, signed) {
		TMat2x2<T> c{m};
		--m;
		return c;
	}
/// @}

/** @name Unary operators */ /// @{
	/// Matrix unary plus.
	template<class T>
	inline TMat2x2<T> operator+(TMat2x2<T> const& m) {
		return TMat2x2<T>{m};
	}

	/// Matrix unary minus.
	template<class T>
	inline TMat2x2<T> operator-(TMat2x2<T> const& m) {
		return TMat2x2<T>::operations::unary_negative(m);
	}
/// @}

/** @name Arithmetic operators */ /// @{
	/// Matrix right-hand value addition (component-wise).
	template<class T>
	inline TMat2x2<T> operator+(TMat2x2<T> const& m, T const& s) {
		return TMat2x2<T>::operations::scalar_add(m, s);
	}

	/// Matrix left-hand value addition (component-wise).
	template<class T>
	inline TMat2x2<T> operator+(T const& s, TMat2x2<T> const& m) {
		return TMat2x2<T>::operations::scalar_add(m, s);
	}

	/// Matrix addition.
	template<class T>
	inline TMat2x2<T> operator+(TMat2x2<T> const& m, TMat2x2<T> const& n) {
		return TMat2x2<T>::operations::add(m, n);
	}

	/// Matrix right-hand value subtraction (component-wise).
	template<class T>
	inline TMat2x2<T> operator-(TMat2x2<T> const& m, T const& s) {
		return TMat2x2<T>::operations::scalar_subtract_rhs(m, s);
	}

	/// Matrix left-hand value subtraction (component-wise).
	template<class T>
	inline TMat2x2<T> operator-(T const& s, TMat2x2<T> const& m) {
		return TMat2x2<T>::operations::scalar_subtract_lhs(m, s);
	}

	/// Matrix subtraction.
	template<class T>
	inline TMat2x2<T> operator-(TMat2x2<T> const& m, TMat2x2<T> const& n) {
		return TMat2x2<T>::operations::subtract(m, n);
	}

	/// Matrix right-hand scalar multiplication (component-wise).
	template<class T>
	inline TMat2x2<T> operator*(TMat2x2<T> const& m, T const& s) {
		return TMat2x2<T>::operations::scalar_multiply(m, s);
	}

	/// Matrix left-hand scalar multiplication (component-wise).
	template<class T>
	inline TMat2x2<T> operator*(T const& s, TMat2x2<T> const& m) {
		return TMat2x2<T>::operations::scalar_multiply(m, s);
	}

	/// Matrix right-hand (row) vector multiplication (proper product).
	template<class T>
	inline typename TMat2x2<T>::col_type
	operator*(TMat2x2<T> const& m, typename TMat2x2<T>::row_type const& v) {
		return TMat2x2<T>::operations::row_multiply(m, v);
	}

	/// Matrix left-hand (column) vector multiplication (proper product).
	template<class T>
	inline typename TMat2x2<T>::row_type
	operator*(typename TMat2x2<T>::col_type const& v, TMat2x2<T> const& m) {
		return TMat2x2<T>::operations::col_multiply(m, v);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat2x2<T> operator*(TMat2x2<T> const& m, TMat2x2<T> const& n) {
		return TMat2x2<T>::operations::multiply(m, n);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat3x2<T> operator*(TMat2x2<T> const& m, TMat3x2<T> const& n) {
		return TMat2x2<T>::operations::multiply(m, n);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat4x2<T> operator*(TMat2x2<T> const& m, TMat4x2<T> const& n) {
		return TMat2x2<T>::operations::multiply(m, n);
	}

	/// Matrix right-hand value division (component-wise).
	template<class T>
	inline TMat2x2<T> operator/(TMat2x2<T> const& m, T const& s) {
		return TMat2x2<T>::operations::scalar_divide_rhs(m, s);
	}

	/// Matrix left-hand value division (component-wise).
	template<class T>
	inline TMat2x2<T> operator/(T const& s, TMat2x2<T> const& m) {
		return TMat2x2<T>::operations::scalar_divide_lhs(m, s);
	}

	/// Matrix right-hand vector division (proper quotient).
	template<class T>
	inline typename TMat2x2<T>::col_type
	operator/(TMat2x2<T> const& m, typename TMat2x2<T>::row_type const& v) {
		return TMat2x2<T>::operations::row_divide(m, v);
	}

	/// Matrix left-hand vector division (proper quotient).
	template<class T>
	inline typename TMat2x2<T>::row_type
	operator/(typename TMat2x2<T>::col_type const& v, TMat2x2<T> const& m) {
		return TMat2x2<T>::operations::col_divide(m, v);
	}

	/// Matrix division (proper quotient).
	template<class T>
	inline TMat2x2<T> operator/(TMat2x2<T> const& m, TMat2x2<T> const& n) {
		return TMat2x2<T>::operations::divide(m, n);
	}
/// @}

/** @} */ // end of doc-group lib_core_math_matrix_2x2

} // namespace math
} // namespace togo
