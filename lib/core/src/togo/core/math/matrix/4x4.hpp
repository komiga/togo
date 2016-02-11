#line 2 "togo/core/math/vector/4x4.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 4-by-4 matrix interface.
@ingroup lib_core_types
@ingroup lib_core_math
@ingroup lib_core_math_matrix
@ingroup lib_core_math_matrix_4x4
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/matrix/4x4_type.hpp>

#include <cmath>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_matrix_4x4
	@{
*/

/** @cond INTERNAL */
template<class T>
struct TMat4x4<T>::operations {
	using type = typename TMat4x4<T>::type;
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
			m.data[0].x, m.data[1].x, m.data[2].x, m.data[3].x,
			m.data[0].y, m.data[1].y, m.data[2].y, m.data[3].y,
			m.data[0].z, m.data[1].z, m.data[2].z, m.data[3].z,
			m.data[0].w, m.data[1].w, m.data[2].w, m.data[3].w
		};
	}

	static value_type determinant(type_cref m) {
		value_type c00=m.data[2].z * m.data[3].w - m.data[3].z * m.data[2].w; // (kp - lo)
		value_type c01=m.data[2].y * m.data[3].w - m.data[3].y * m.data[2].w; // (gp - ho)
		value_type c02=m.data[2].y * m.data[3].z - m.data[3].y * m.data[2].z; // (gl - hk)
		value_type c03=m.data[2].x * m.data[3].w - m.data[3].x * m.data[2].w; // (cp - do)
		value_type c04=m.data[2].x * m.data[3].z - m.data[3].x * m.data[2].z; // (cl - dk)
		value_type c05=m.data[2].x * m.data[3].y - m.data[3].x * m.data[2].y; // (ch - dg)

		row_type dc{
			 (m.data[1].y * c00 - m.data[1].z * c01 + m.data[1].w * c02), // f(kp - lo) - j(gp - ho) + n(gl - hk)
			-(m.data[1].x * c00 - m.data[1].z * c03 + m.data[1].w * c04), // b(kp - lo) - j(cp - do) + n(cl - dk)
			+(m.data[1].x * c01 - m.data[1].y * c03 + m.data[1].w * c05), // b(gp - ho) - f(cp - do) + n(ch - dg)
			-(m.data[1].x * c02 - m.data[1].y * c04 + m.data[1].z * c05)} // b(gl - hk) - f(cl - dk) + j(ch - dg)
		;

		return
			m.data[0].x * dc.x + // a(f(kp - lo) - j(gp - ho) + n(gl - hk)) +
			m.data[0].y * dc.y + // e(b(kp - lo) - j(cp - do) + n(cl - dk)) +
			m.data[0].z * dc.z + // i(b(gp - ho) - f(cp - do) + n(ch - dg)) +
			m.data[0].w * dc.w   // m(b(gl - hk) - f(cl - dk) + j(ch - dg))
		;
	}

	static type inverse(type_cref m) {
		value_type c00 = m.data[2].z * m.data[3].w - m.data[3].z * m.data[2].w; // (kp - lo)
		value_type c02 = m.data[1].z * m.data[3].w - m.data[3].z * m.data[1].w; // (jp - ln)
		value_type c03 = m.data[1].z * m.data[2].w - m.data[2].z * m.data[1].w; // (jl - kn)

		value_type c04 = m.data[2].y * m.data[3].w - m.data[3].y * m.data[2].w; // (gp - ho)
		value_type c06 = m.data[1].y * m.data[3].w - m.data[3].y * m.data[1].w; // (fp - hn)
		value_type c07 = m.data[1].y * m.data[2].w - m.data[2].y * m.data[1].w; // (fl - gn)

		value_type c08 = m.data[2].y * m.data[3].z - m.data[3].y * m.data[2].z; // (gl - hk)
		value_type c10 = m.data[1].y * m.data[3].z - m.data[3].y * m.data[1].z; // (fl - hj)
		value_type c11 = m.data[1].y * m.data[2].z - m.data[2].y * m.data[1].z; // (fk - gj)

		value_type c12 = m.data[2].x * m.data[3].w - m.data[3].x * m.data[2].w; // (cp - do)
		value_type c14 = m.data[1].x * m.data[3].w - m.data[3].x * m.data[1].w; // (bp - dn)
		value_type c15 = m.data[1].x * m.data[2].w - m.data[2].x * m.data[1].w; // (bl - cn)

		value_type c16 = m.data[2].x * m.data[3].z - m.data[3].x * m.data[2].z; // (cl - dk)
		value_type c18 = m.data[1].x * m.data[3].z - m.data[3].x * m.data[1].z; // (bl - dj)
		value_type c19 = m.data[1].x * m.data[2].z - m.data[2].x * m.data[1].z; // (bk - cj)

		value_type c20 = m.data[2].x * m.data[3].y - m.data[3].x * m.data[2].y; // (ch - dg)
		value_type c22 = m.data[1].x * m.data[3].y - m.data[3].x * m.data[1].y; // (bh - df)
		value_type c23 = m.data[1].x * m.data[2].y - m.data[2].x * m.data[1].y; // (bg - cf)

		TVec4<T> f0{c00, c00, c02, c03};
		TVec4<T> f1{c04, c04, c06, c07};
		TVec4<T> f2{c08, c08, c10, c11};
		TVec4<T> f3{c12, c12, c14, c15};
		TVec4<T> f4{c16, c16, c18, c19};
		TVec4<T> f5{c20, c20, c22, c23};

		TVec4<T> v0{m.data[1].x, m.data[0].x, m.data[0].x, m.data[0].x};
		TVec4<T> v1{m.data[1].y, m.data[0].y, m.data[0].y, m.data[0].y};
		TVec4<T> v2{m.data[1].z, m.data[0].z, m.data[0].z, m.data[0].z};
		TVec4<T> v3{m.data[1].w, m.data[0].w, m.data[0].w, m.data[0].w};

		TVec4<T> const sa{+1,-1, +1,-1};
		TVec4<T> const sb{-1,+1, -1,+1};
		type invm{
			sa * (v1 * f0 - v2 * f1 + v3 * f2),
			sb * (v0 * f0 - v2 * f3 + v3 * f4),
			sa * (v0 * f1 - v1 * f3 + v3 * f5),
			sb * (v0 * f2 - v1 * f4 + v2 * f5)
		};

		value_type const det = row_type::operations::dot(
			m.data[0],
			row_type{
				invm.data[0].x, invm.data[1].x, invm.data[2].x, invm.data[3].x
			}
		);
		invm /= det;
		return invm;
	}

	static type unary_negative(type_cref m) {
		return type{
			-m.data[0],
			-m.data[1],
			-m.data[2],
			-m.data[3]
		};
	}

	static type scalar_add(type_cref m, value_cref s) {
		return type{
			m.data[0] + s,
			m.data[1] + s,
			m.data[2] + s,
			m.data[3] + s
		};
	}

	static type add(type_cref m, type_cref n) {
		return type{
			m.data[0] + n.data[0],
			m.data[1] + n.data[1],
			m.data[2] + n.data[2],
			m.data[3] + n.data[3]
		};
	}

	static type scalar_subtract_rhs(type_cref m, value_cref s) {
		return type{
			m.data[0] - s,
			m.data[1] - s,
			m.data[2] - s,
			m.data[3] - s
		};
	}

	static type scalar_subtract_lhs(type_cref m, value_cref s) {
		return type{
			s - m.data[0],
			s - m.data[1],
			s - m.data[2],
			s - m.data[3]
		};
	}

	static type subtract(type_cref m, type_cref n) {
		return type{
			m.data[0] - n.data[0],
			m.data[1] - n.data[1],
			m.data[2] - n.data[2],
			m.data[3] - n.data[3]
		};
	}

	static type scalar_multiply(type_cref m, value_cref s) {
		return type{
			m.data[0] * s,
			m.data[1] * s,
			m.data[2] * s,
			m.data[3] * s
		};
	}

	static col_type row_multiply(type_cref m, row_cref v) {
		return col_type{
			m.data[0].x * v.x + m.data[1].x * v.y + m.data[2].x * v.z + m.data[3].x * v.w,
			m.data[0].y * v.x + m.data[1].y * v.y + m.data[2].y * v.z + m.data[3].y * v.w,
			m.data[0].z * v.x + m.data[1].z * v.y + m.data[2].z * v.z + m.data[3].z * v.w,
			m.data[0].w * v.x + m.data[1].w * v.y + m.data[2].w * v.z + m.data[3].w * v.w
		};
	}

	static row_type col_multiply(type_cref m, col_cref v) {
		return row_type{
			m.data[0].x * v.x + m.data[0].y * v.y + m.data[0].z * v.z + m.data[0].w * v.w,
			m.data[1].x * v.x + m.data[1].y * v.y + m.data[1].z * v.z + m.data[1].w * v.w,
			m.data[2].x * v.x + m.data[2].y * v.y + m.data[2].z * v.z + m.data[2].w * v.w,
			m.data[3].x * v.x + m.data[3].y * v.y + m.data[3].z * v.z + m.data[3].w * v.w
		};
	}

	static TMat2x4<T> multiply(type_cref m, TMat2x4<T> const& n) {
		return TMat2x4<T>{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y + m.data[2].x * n.data[0].z + m.data[3].x * n.data[0].w,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y + m.data[2].y * n.data[0].z + m.data[3].y * n.data[0].w,
			m.data[0].z * n.data[0].x + m.data[1].z * n.data[0].y + m.data[2].z * n.data[0].z + m.data[3].z * n.data[0].w,
			m.data[0].w * n.data[0].x + m.data[1].w * n.data[0].y + m.data[2].w * n.data[0].z + m.data[3].w * n.data[0].w,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y + m.data[2].x * n.data[1].z + m.data[3].x * n.data[1].w,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y + m.data[2].y * n.data[1].z + m.data[3].y * n.data[1].w,
			m.data[0].z * n.data[1].x + m.data[1].z * n.data[1].y + m.data[2].z * n.data[1].z + m.data[3].z * n.data[1].w,
			m.data[0].w * n.data[1].x + m.data[1].w * n.data[1].y + m.data[2].w * n.data[1].z + m.data[3].w * n.data[1].w
		};
	}

	static TMat3x4<T> multiply(type_cref m, TMat3x4<T> const& n) {
		return TMat3x4<T>{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y + m.data[2].x * n.data[0].z + m.data[3].x * n.data[0].w,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y + m.data[2].y * n.data[0].z + m.data[3].y * n.data[0].w,
			m.data[0].z * n.data[0].x + m.data[1].z * n.data[0].y + m.data[2].z * n.data[0].z + m.data[3].z * n.data[0].w,
			m.data[0].w * n.data[0].x + m.data[1].w * n.data[0].y + m.data[2].w * n.data[0].z + m.data[3].w * n.data[0].w,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y + m.data[2].x * n.data[1].z + m.data[3].x * n.data[1].w,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y + m.data[2].y * n.data[1].z + m.data[3].y * n.data[1].w,
			m.data[0].z * n.data[1].x + m.data[1].z * n.data[1].y + m.data[2].z * n.data[1].z + m.data[3].z * n.data[1].w,
			m.data[0].w * n.data[1].x + m.data[1].w * n.data[1].y + m.data[2].w * n.data[1].z + m.data[3].w * n.data[1].w,
			m.data[0].x * n.data[2].x + m.data[1].x * n.data[2].y + m.data[2].x * n.data[2].z + m.data[3].x * n.data[2].w,
			m.data[0].y * n.data[2].x + m.data[1].y * n.data[2].y + m.data[2].y * n.data[2].z + m.data[3].y * n.data[2].w,
			m.data[0].z * n.data[2].x + m.data[1].z * n.data[2].y + m.data[2].z * n.data[2].z + m.data[3].z * n.data[2].w,
			m.data[0].w * n.data[2].x + m.data[1].w * n.data[2].y + m.data[2].w * n.data[2].z + m.data[3].w * n.data[2].w
		};
	}

	static type multiply(type_cref m, type_cref n) {
		return type{
			m.data[0].x * n.data[0].x + m.data[1].x * n.data[0].y + m.data[2].x * n.data[0].z + m.data[3].x * n.data[0].w,
			m.data[0].y * n.data[0].x + m.data[1].y * n.data[0].y + m.data[2].y * n.data[0].z + m.data[3].y * n.data[0].w,
			m.data[0].z * n.data[0].x + m.data[1].z * n.data[0].y + m.data[2].z * n.data[0].z + m.data[3].z * n.data[0].w,
			m.data[0].w * n.data[0].x + m.data[1].w * n.data[0].y + m.data[2].w * n.data[0].z + m.data[3].w * n.data[0].w,
			m.data[0].x * n.data[1].x + m.data[1].x * n.data[1].y + m.data[2].x * n.data[1].z + m.data[3].x * n.data[1].w,
			m.data[0].y * n.data[1].x + m.data[1].y * n.data[1].y + m.data[2].y * n.data[1].z + m.data[3].y * n.data[1].w,
			m.data[0].z * n.data[1].x + m.data[1].z * n.data[1].y + m.data[2].z * n.data[1].z + m.data[3].z * n.data[1].w,
			m.data[0].w * n.data[1].x + m.data[1].w * n.data[1].y + m.data[2].w * n.data[1].z + m.data[3].w * n.data[1].w,
			m.data[0].x * n.data[2].x + m.data[1].x * n.data[2].y + m.data[2].x * n.data[2].z + m.data[3].x * n.data[2].w,
			m.data[0].y * n.data[2].x + m.data[1].y * n.data[2].y + m.data[2].y * n.data[2].z + m.data[3].y * n.data[2].w,
			m.data[0].z * n.data[2].x + m.data[1].z * n.data[2].y + m.data[2].z * n.data[2].z + m.data[3].z * n.data[2].w,
			m.data[0].w * n.data[2].x + m.data[1].w * n.data[2].y + m.data[2].w * n.data[2].z + m.data[3].w * n.data[2].w,
			m.data[0].x * n.data[3].x + m.data[1].x * n.data[3].y + m.data[2].x * n.data[3].z + m.data[3].x * n.data[3].w,
			m.data[0].y * n.data[3].x + m.data[1].y * n.data[3].y + m.data[2].y * n.data[3].z + m.data[3].y * n.data[3].w,
			m.data[0].z * n.data[3].x + m.data[1].z * n.data[3].y + m.data[2].z * n.data[3].z + m.data[3].z * n.data[3].w,
			m.data[0].w * n.data[3].x + m.data[1].w * n.data[3].y + m.data[2].w * n.data[3].z + m.data[3].w * n.data[3].w
		};
	}

	static type scalar_divide_rhs(type_cref m, value_cref s) {
		return type{
			m.data[0] / s,
			m.data[1] / s,
			m.data[2] / s,
			m.data[3] / s
		};
	}

	static type scalar_divide_lhs(type_cref m, value_cref s) {
		return type{
			s / m.data[0],
			s / m.data[1],
			s / m.data[2],
			s / m.data[3]
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
	inline bool operator==(TMat4x4<T> const& m, TMat4x4<T> const& n) {
		return
			m.data[0] == n.data[0] &&
			m.data[1] == n.data[1] &&
			m.data[2] == n.data[2] &&
			m.data[3] == n.data[3];
	}

	/// Non-equivalence operator.
	template<class T>
	inline bool operator!=(TMat4x4<T> const& m, TMat4x4<T> const& n) {
		return
			m.data[0] != n.data[0] ||
			m.data[1] != n.data[1] ||
			m.data[2] != n.data[2] ||
			m.data[3] != n.data[3];
	}
/// @}
#pragma GCC diagnostic pop

/** @name Arithmetic assignment operators */ /// @{
	/// Add value to all components.
	template<class T, class U>
	inline TMat4x4<T>& operator+=(TMat4x4<T>& m, U const& s) {
		m.data[0] += T(s);
		m.data[1] += T(s);
		m.data[2] += T(s);
		m.data[3] += T(s);
		return m;
	}

	/// Add matrix.
	template<class T, class U>
	inline TMat4x4<T>& operator+=(TMat4x4<T>& m, TMat4x4<U> const& n) {
		m.data[0] += n.data[0];
		m.data[1] += n.data[1];
		m.data[2] += n.data[2];
		m.data[3] += n.data[3];
		return m;
	}

	/// Subtract value from all components.
	template<class T, class U>
	inline TMat4x4<T>& operator-=(TMat4x4<T>& m, U const& s) {
		m.data[0] -= T(s);
		m.data[1] -= T(s);
		m.data[2] -= T(s);
		m.data[3] -= T(s);
		return m;
	}

	/// Subtract matrix.
	template<class T, class U>
	inline TMat4x4<T>& operator-=(TMat4x4<T>& m, TMat4x4<U> const& n) {
		m.data[0] -= n.data[0];
		m.data[1] -= n.data[1];
		m.data[2] -= n.data[2];
		m.data[3] -= n.data[3];
		return m;
	}

	/// Multiply by scalar.
	template<class T, class U>
	inline TMat4x4<T>& operator*=(TMat4x4<T>& m, U const& s) {
		m.data[0] *= T(s);
		m.data[1] *= T(s);
		m.data[2] *= T(s);
		m.data[3] *= T(s);
		return m;
	}

	/// Multiply by matrix (proper product).
	template<class T, class U>
	inline TMat4x4<T>& operator*=(TMat4x4<T>& m, TMat4x4<U> const& n) {
		return (m = m * n);
	}

	/// Divide all components by value.
	template<class T, class U>
	inline TMat4x4<T>& operator/=(TMat4x4<T>& m, U const& s) {
		m.data[0] /= T(s);
		m.data[1] /= T(s);
		m.data[2] /= T(s);
		m.data[3] /= T(s);
		return m;
	}

	/// Divide by matrix (proper quotient).
	template<class T, class U>
	inline TMat4x4<T>& operator/=(TMat4x4<T>& m, TMat4x4<U> const& n) {
		return (m = m / n);
	}
/// @}

/** @name Increment and decrement operators */ /// @{
	/// Prefix increment.
	template<class T>
	inline TMat4x4<T>& operator++(TMat4x4<T>& m) {
		++m.data[0];
		++m.data[1];
		++m.data[2];
		++m.data[3];
		return m;
	}

	/// Prefix decrement.
	template<class T>
	inline TMat4x4<T>& operator--(TMat4x4<T>& m) {
		--m.data[0];
		--m.data[1];
		--m.data[2];
		--m.data[3];
		return m;
	}

	/// Matrix postfix increment.
	template<class T>
	inline TMat4x4<T> operator++(TMat4x4<T>& m, signed) {
		TMat4x4<T> c{m};
		++m;
		return c;
	}

	/// Matrix postfix decrement.
	template<class T>
	inline TMat4x4<T> operator--(TMat4x4<T>& m, signed) {
		TMat4x4<T> c{m};
		--m;
		return c;
	}
/// @}

/** @name Unary operators */ /// @{
	/// Matrix unary plus.
	template<class T>
	inline TMat4x4<T> operator+(TMat4x4<T> const& m) {
		return TMat4x4<T>{m};
	}

	/// Matrix unary minus.
	template<class T>
	inline TMat4x4<T> operator-(TMat4x4<T> const& m) {
		return TMat4x4<T>::operations::unary_negative(m);
	}
/// @}

/** @name Arithmetic operators */ /// @{
	/// Matrix right-hand value addition (component-wise).
	template<class T>
	inline TMat4x4<T> operator+(TMat4x4<T> const& m, T const& s) {
		return TMat4x4<T>::operations::scalar_add(m, s);
	}

	/// Matrix left-hand value addition (component-wise).
	template<class T>
	inline TMat4x4<T> operator+(T const& s, TMat4x4<T> const& m) {
		return TMat4x4<T>::operations::scalar_add(m, s);
	}

	/// Matrix addition.
	template<class T>
	inline TMat4x4<T> operator+(TMat4x4<T> const& m, TMat4x4<T> const& n) {
		return TMat4x4<T>::operations::add(m, n);
	}

	/// Matrix right-hand value subtraction (component-wise).
	template<class T>
	inline TMat4x4<T> operator-(TMat4x4<T> const& m, T const& s) {
		return TMat4x4<T>::operations::scalar_subtract_rhs(m, s);
	}

	/// Matrix left-hand value subtraction (component-wise).
	template<class T>
	inline TMat4x4<T> operator-(T const& s, TMat4x4<T> const& m) {
		return TMat4x4<T>::operations::scalar_subtract_lhs(m, s);
	}

	/// Matrix subtraction.
	template<class T>
	inline TMat4x4<T> operator-(TMat4x4<T> const& m, TMat4x4<T> const& n) {
		return TMat4x4<T>::operations::subtract(m, n);
	}

	/// Matrix right-hand scalar multiplication (component-wise).
	template<class T>
	inline TMat4x4<T> operator*(TMat4x4<T> const& m, T const& s) {
		return TMat4x4<T>::operations::scalar_multiply(m, s);
	}

	/// Matrix left-hand scalar multiplication (component-wise).
	template<class T>
	inline TMat4x4<T> operator*(T const& s, TMat4x4<T> const& m) {
		return TMat4x4<T>::operations::scalar_multiply(m, s);
	}

	/// Matrix right-hand (row) vector multiplication (proper product).
	template<class T>
	inline typename TMat4x4<T>::col_type
	operator*(TMat4x4<T> const& m, typename TMat4x4<T>::row_type const& v) {
		return TMat4x4<T>::operations::row_multiply(m, v);
	}

	/// Matrix left-hand (column) vector multiplication (proper product).
	template<class T>
	inline typename TMat4x4<T>::row_type
	operator*(typename TMat4x4<T>::col_type const& v, TMat4x4<T> const& m) {
		return TMat4x4<T>::operations::col_multiply(m, v);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat2x4<T> operator*(TMat4x4<T> const& m, TMat2x4<T> const& n) {
		return TMat4x4<T>::operations::multiply(m, n);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat3x4<T> operator*(TMat4x4<T> const& m, TMat3x4<T> const& n) {
		return TMat4x4<T>::operations::multiply(m, n);
	}

	/// Matrix multiplication (proper product).
	template<class T>
	inline TMat4x4<T> operator*(TMat4x4<T> const& m, TMat4x4<T> const& n) {
		return TMat4x4<T>::operations::multiply(m, n);
	}

	/// Matrix right-hand value division (component-wise).
	template<class T>
	inline TMat4x4<T> operator/(TMat4x4<T> const& m, T const& s) {
		return TMat4x4<T>::operations::scalar_divide_rhs(m, s);
	}

	/// Matrix left-hand value division (component-wise).
	template<class T>
	inline TMat4x4<T> operator/(T const& s, TMat4x4<T> const& m) {
		return TMat4x4<T>::operations::scalar_divide_lhs(m, s);
	}

	/// Matrix right-hand vector division (proper quotient).
	template<class T>
	inline typename TMat4x4<T>::col_type
	operator/(TMat4x4<T> const& m, typename TMat4x4<T>::row_type const& v) {
		return TMat4x4<T>::operations::row_divide(m, v);
	}

	/// Matrix left-hand vector division (proper quotient).
	template<class T>
	inline typename TMat4x4<T>::row_type
	operator/(typename TMat4x4<T>::col_type const& v, TMat4x4<T> const& m) {
		return TMat4x4<T>::operations::col_divide(m, v);
	}

	/// Matrix division (proper quotient).
	template<class T>
	inline TMat4x4<T> operator/(TMat4x4<T> const& m, TMat4x4<T> const& n) {
		return TMat4x4<T>::operations::divide(m, n);
	}
/// @}

/** @} */ // end of doc-group lib_core_math_matrix_4x4

} // namespace math
} // namespace togo
