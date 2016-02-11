#line 2 "togo/core/math/vector/3x3.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 3-by-3 matrix interface.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_matrix
@ingroup lib_core_math_matrix_3x3
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/matrix/3x3_type.hpp>

#include <cmath>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_matrix_3x3
	@{
*/

/** @cond INTERNAL */
template<class T>
struct TMat3x3<T>::operations {
	using type = typename TMat3x3<T>::type;
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
			m.data[0].x, m.data[1].x, m.data[2].x,
			m.data[0].y, m.data[1].y, m.data[2].y,
			m.data[0].z, m.data[1].z, m.data[2].z
		};
	}

	static value_type determinant(type_cref m) {
		return
			m.data[0].x * (m.data[1].y * m.data[2].z - m.data[2].y * m.data[1].z) - // a(ei - fh) -
			m.data[1].x * (m.data[0].y * m.data[2].z - m.data[2].y * m.data[0].z) + // b(di - fg) +
			m.data[2].x * (m.data[0].y * m.data[1].z - m.data[1].y * m.data[0].z)   // c(dh - eg)
		;
	}

	static type inverse(type_cref m) {
		/*
			a b c
			d e f
			g h i
		*/
		value_type const
		c01{m.data[1].y * m.data[2].z - m.data[2].y * m.data[1].z}, // (ei - fh)
		c02{m.data[0].y * m.data[2].z - m.data[2].y * m.data[0].z}, // (fg - di)
		c03{m.data[0].y * m.data[1].z - m.data[1].y * m.data[0].z}, // (dh - eg)
		c11{m.data[1].x * m.data[2].z - m.data[2].x * m.data[1].z}, // (ch - bi)
		c12{m.data[0].x * m.data[2].z - m.data[2].x * m.data[0].z}, // (ai - cg)
		c13{m.data[0].x * m.data[1].z - m.data[1].x * m.data[0].z}, // (bg - ah)
		c21{m.data[1].x * m.data[2].y - m.data[2].x * m.data[1].y}, // (bf - ce)
		c22{m.data[0].x * m.data[2].y - m.data[2].x * m.data[0].y}, // (cd - af)
		c23{m.data[0].x * m.data[1].y - m.data[1].x * m.data[0].y}; // (ae - bd)

		value_type const det=
			m.data[0].x * c01 - // a(ei - fh) -
			m.data[1].x * c02 + // b(di - fg) +
			m.data[2].x * c03 ; // c(dh - eg)
		return type{
			 c01 / det, // (ei - fh)
			-c02 / det, //-(di - fg)
			 c03 / det, // (dh - eg)
			-c11 / det, //-(bi - ch)
			 c12 / det, // (ai - cg)
			-c13 / det, //-(ah - bg)
			 c21 / det, // (bf - ce)
			-c22 / det, //-(af - cd)
			 c23 / det  // (ae - bd)
		
		};
	}

	static type unary_negative(type_cref m) {
		return type{
			-m.data[0],
			-m.data[1],
			-m.data[2]
		};
	}

	static type scalar_add(type_cref m, value_cref s) {
		return type{
			m.data[0] + s,
			m.data[1] + s,
			m.data[2] + s
		};
	}

	static type add(type_cref m, type_cref n) {
		return type{
			m.data[0] + n.data[0],
			m.data[1] + n.data[1],
			m.data[2] + n.data[2]
		};
	}

	static type scalar_subtract_rhs(type_cref m, value_cref s) {
		return type{
			m.data[0] - s,
			m.data[1] - s,
			m.data[2] - s
		};
	}

	static type scalar_subtract_lhs(type_cref m, value_cref s) {
		return type{
			s - m.data[0],
			s - m.data[1],
			s - m.data[2]
		};
	}

	static type subtract(type_cref m, type_cref n) {
		return type{
			m.data[0] - n.data[0],
			m.data[1] - n.data[1],
			m.data[2] - n.data[2]
		};
	}

	static type scalar_multiply(type_cref m, value_cref s) {
		return type{
			m.data[0] * s,
			m.data[1] * s,
			m.data[2] * s
		};
	}

	static col_type row_multiply(type_cref m, row_cref v) {
		return col_type{
			m.data[0].x * v.x + m.data[1].x * v.y + m.data[2].x * v.z,
			m.data[0].y * v.x + m.data[1].y * v.y + m.data[2].y * v.z,
			m.data[0].z * v.x + m.data[1].z * v.y + m.data[2].z * v.z
		};
	}

	static row_type col_multiply(type_cref m, col_cref v) {
		return row_type{
			m.data[0].x * v.x + m.data[0].y * v.y + m.data[0].z * v.z,
			m.data[1].x * v.x + m.data[1].y * v.y + m.data[1].z * v.z,
			m.data[2].x * v.x + m.data[2].y * v.y + m.data[2].z * v.z
		};
	}

	static TMat2x3<T> multiply(type_cref m, TMat2x3<T> const& n) {
		return TMat2x3<T>{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y + m.data[2].x * n.data[0].z,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y + m.data[2].y * n.data[0].z,
			m.data[0].z * n.data[0].x + m.data[1].z * n.data[0].y + m.data[2].z * n.data[0].z,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y + m.data[2].x * n.data[1].z,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y + m.data[2].y * n.data[1].z,
			m.data[0].z * n.data[1].x + m.data[1].z * n.data[1].y + m.data[2].z * n.data[1].z
		};
	}

	static type multiply(type_cref m, type_cref n) {
		return type{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y + m.data[2].x * n.data[0].z,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y + m.data[2].y * n.data[0].z,
			m.data[0].z * n.data[0].x + m.data[1].z * n.data[0].y + m.data[2].z * n.data[0].z,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y + m.data[2].x * n.data[1].z,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y + m.data[2].y * n.data[1].z,
			m.data[0].z * n.data[1].x + m.data[1].z * n.data[1].y + m.data[2].z * n.data[1].z,
			m.data[0].x * n.data[2].x + m.data[1].x * n.data[2].y + m.data[2].x * n.data[2].z,
			m.data[0].y * n.data[2].x + m.data[1].y * n.data[2].y + m.data[2].y * n.data[2].z,
			m.data[0].z * n.data[2].x + m.data[1].z * n.data[2].y + m.data[2].z * n.data[2].z
		};
	}

	static TMat4x3<T> multiply(type_cref m, TMat4x3<T> const& n) {
		return TMat4x3<T>{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y + m.data[2].x * n.data[0].z,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y + m.data[2].y * n.data[0].z,
			m.data[0].z * n.data[0].x + m.data[1].z * n.data[0].y + m.data[2].z * n.data[0].z,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y + m.data[2].x * n.data[1].z,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y + m.data[2].y * n.data[1].z,
			m.data[0].z * n.data[1].x + m.data[1].z * n.data[1].y + m.data[2].z * n.data[1].z,
			m.data[0].x * n.data[2].x + m.data[1].x * n.data[2].y + m.data[2].x * n.data[2].z,
			m.data[0].y * n.data[2].x + m.data[1].y * n.data[2].y + m.data[2].y * n.data[2].z,
			m.data[0].z * n.data[2].x + m.data[1].z * n.data[2].y + m.data[2].z * n.data[2].z,
			m.data[0].x * n.data[3].x + m.data[1].x * n.data[3].y + m.data[2].x * n.data[3].z,
			m.data[0].y * n.data[3].x + m.data[1].y * n.data[3].y + m.data[2].y * n.data[3].z,
			m.data[0].z * n.data[3].x + m.data[1].z * n.data[3].y + m.data[2].z * n.data[3].z
		};
	}

	static type scalar_divide_rhs(type_cref m, value_cref s) {
		return type{
			m.data[0] / s,
			m.data[1] / s,
			m.data[2] / s
		};
	}

	static type scalar_divide_lhs(type_cref m, value_cref s) {
		return type{
			s / m.data[0],
			s / m.data[1],
			s / m.data[2]
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
	inline bool operator==(TMat3x3<T> const& m, TMat3x3<T> const& n) {
		return
			m.data[0] == n.data[0] &&
			m.data[1] == n.data[1] &&
			m.data[2] == n.data[2]
		;
	}

	/// Non-equivalence operator.
	template<class T>
	inline bool operator!=(TMat3x3<T> const& m, TMat3x3<T> const& n) {
		return
			m.data[0] != n.data[0] ||
			m.data[1] != n.data[1] ||
			m.data[2] != n.data[2]
		;
	}
/// @}
#pragma GCC diagnostic pop

/** @name Arithmetic assignment operators */ /// @{
	/// Add value to all components.
	template<class T, class U>
	inline TMat3x3<T>& operator+=(TMat3x3<T>& m, U const& s) {
		m.data[0] += T(s);
		m.data[1] += T(s);
		m.data[2] += T(s);
		return m;
	}

	/// Add matrix.
	template<class T, class U>
	inline TMat3x3<T>& operator+=(TMat3x3<T>& m, TMat3x3<U> const& n) {
		m.data[0] += n.data[0];
		m.data[1] += n.data[1];
		m.data[2] += n.data[2];
		return m;
	}

	/// Subtract value from all components.
	template<class T, class U>
	inline TMat3x3<T>& operator-=(TMat3x3<T>& m, U const& s) {
		m.data[0] -= T(s);
		m.data[1] -= T(s);
		m.data[2] -= T(s);
		return m;
	}

	/// Subtract matrix.
	template<class T, class U>
	inline TMat3x3<T>& operator-=(TMat3x3<T>& m, TMat3x3<U> const& n) {
		m.data[0] -= n.data[0];
		m.data[1] -= n.data[1];
		m.data[2] -= n.data[2];
		return m;
	}

	/// Multiply by scalar.
	template<class T, class U>
	inline TMat3x3<T>& operator*=(TMat3x3<T>& m, U const& s) {
		m.data[0] *= T(s);
		m.data[1] *= T(s);
		m.data[2] *= T(s);
		return m;
	}

	/// Multiply by matrix (proper product).
	template<class T, class U>
	inline TMat3x3<T>& operator*=(TMat3x3<T>& m, TMat3x3<U> const& n) {
		return (m = m * n);
	}

	/// Divide all components by value.
	template<class T, class U>
	inline TMat3x3<T>& operator/=(TMat3x3<T>& m, U const& s) {
		m.data[0] /= T(s);
		m.data[1] /= T(s);
		m.data[2] /= T(s);
		return m;
	}

	/// Divide by matrix (proper quotient).
	template<class T, class U>
	inline TMat3x3<T>& operator/=(TMat3x3<T>& m, TMat3x3<U> const& n) {
		return (m = m / n);
	}
/// @}

/** @name Increment and decrement operators */ /// @{
	/// Prefix increment.
	template<class T>
	inline TMat3x3<T>& operator++(TMat3x3<T>& m) {
		++m.data[0];
		++m.data[1];
		++m.data[2];
		return m;
	}

	/// Prefix decrement.
	template<class T>
	inline TMat3x3<T>& operator--(TMat3x3<T>& m) {
		--m.data[0];
		--m.data[1];
		--m.data[2];
		return m;
	}

	/// Matrix postfix increment.
	template<class T>
	inline TMat3x3<T> operator++(TMat3x3<T>& m, signed) {
		TMat3x3<T> c{m};
		++m;
		return c;
	}

	/// Matrix postfix decrement.
	template<class T>
	inline TMat3x3<T> operator--(TMat3x3<T>& m, signed) {
		TMat3x3<T> c{m};
		--m;
		return c;
	}
/// @}

/** @name Unary operators */ /// @{
	/// Matrix unary plus.
	template<class T>
	inline TMat3x3<T> operator+(TMat3x3<T> const& m) {
		return TMat3x3<T>{m};
	}

	/// Matrix unary minus.
	template<class T>
	inline TMat3x3<T> operator-(TMat3x3<T> const& m) {
		return TMat3x3<T>::operations::unary_negative(m);
	}
/// @}

/** @name Arithmetic operators */ /// @{
	/// Matrix right-hand value addition (component-wise).
	template<class T>
	inline TMat3x3<T> operator+(TMat3x3<T> const& m, T const& s) {
		return TMat3x3<T>::operations::scalar_add(m, s);
	}

	/// Matrix left-hand value addition (component-wise).
	template<class T>
	inline TMat3x3<T> operator+(T const& s, TMat3x3<T> const& m) {
		return TMat3x3<T>::operations::scalar_add(m, s);
	}

	/// Matrix addition.
	template<class T>
	inline TMat3x3<T> operator+(TMat3x3<T> const& m, TMat3x3<T> const& n) {
		return TMat3x3<T>::operations::add(m, n);
	}

	/// Matrix right-hand value subtraction (component-wise).
	template<class T>
	inline TMat3x3<T> operator-(TMat3x3<T> const& m, T const& s) {
		return TMat3x3<T>::operations::scalar_subtract_rhs(m, s);
	}

	/// Matrix left-hand value subtraction (component-wise).
	template<class T>
	inline TMat3x3<T> operator-(T const& s, TMat3x3<T> const& m) {
		return TMat3x3<T>::operations::scalar_subtract_lhs(m, s);
	}

	/// Matrix subtraction.
	template<class T>
	inline TMat3x3<T> operator-(TMat3x3<T> const& m, TMat3x3<T> const& n) {
		return TMat3x3<T>::operations::subtract(m, n);
	}

	/// Matrix right-hand scalar multiplication (component-wise).
	template<class T>
	inline TMat3x3<T> operator*(TMat3x3<T> const& m, T const& s) {
		return TMat3x3<T>::operations::scalar_multiply(m, s);
	}

	/// Matrix left-hand scalar multiplication (component-wise).
	template<class T>
	inline TMat3x3<T> operator*(T const& s, TMat3x3<T> const& m) {
		return TMat3x3<T>::operations::scalar_multiply(m, s);
	}

	/// Matrix right-hand (row) vector multiplication (proper product).
	template<class T>
	inline typename TMat3x3<T>::col_type
	operator*(TMat3x3<T> const& m, typename TMat3x3<T>::row_type const& v) {
		return TMat3x3<T>::operations::row_multiply(m, v);
	}

	/// Matrix left-hand (column) vector multiplication (proper product).
	template<class T>
	inline typename TMat3x3<T>::row_type
	operator*(typename TMat3x3<T>::col_type const& v, TMat3x3<T> const& m) {
		return TMat3x3<T>::operations::col_multiply(m, v);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat2x3<T> operator*(TMat3x3<T> const& m, TMat2x3<T> const& n) {
		return TMat3x3<T>::operations::multiply(m, n);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat3x3<T> operator*(TMat3x3<T> const& m, TMat3x3<T> const& n) {
		return TMat3x3<T>::operations::multiply(m, n);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat4x3<T> operator*(TMat3x3<T> const& m, TMat4x3<T> const& n) {
		return TMat3x3<T>::operations::multiply(m, n);
	}

	/// Matrix right-hand value division (component-wise).
	template<class T>
	inline TMat3x3<T> operator/(TMat3x3<T> const& m, T const& s) {
		return TMat3x3<T>::operations::scalar_divide_rhs(m, s);
	}

	/// Matrix left-hand value division (component-wise).
	template<class T>
	inline TMat3x3<T> operator/(T const& s, TMat3x3<T> const& m) {
		return TMat3x3<T>::operations::scalar_divide_lhs(m, s);
	}

	/// Matrix right-hand vector division (proper quotient).
	template<class T>
	inline typename TMat3x3<T>::col_type
	operator/(TMat3x3<T> const& m, typename TMat3x3<T>::row_type const& v) {
		return TMat3x3<T>::operations::row_divide(m, v);
	}

	/// Matrix left-hand vector division (proper quotient).
	template<class T>
	inline typename TMat3x3<T>::row_type
	operator/(typename TMat3x3<T>::col_type const& v, TMat3x3<T> const& m) {
		return TMat3x3<T>::operations::col_divide(m, v);
	}

	/// Matrix division (proper quotient).
	template<class T>
	inline TMat3x3<T> operator/(TMat3x3<T> const& m, TMat3x3<T> const& n) {
		return TMat3x3<T>::operations::divide(m, n);
	}
/// @}

/** @} */ // end of doc-group lib_core_math_matrix_3x3

} // namespace math
} // namespace togo
