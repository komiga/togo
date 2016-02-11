#line 2 "togo/core/math/vector/1.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 1-dimensional vector interface.
@ingroup lib_core_math
@ingroup lib_core_math_vector
@ingroup lib_core_math_vector_1d
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/vector/1_type.hpp>

#include <cmath>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_vector_1d
	@{
*/

/** @cond INTERNAL */
template<class T>
struct TVec1<T>::operations {
	using type = typename TVec1<T>::type;
	using type_cref = type const&;
	using value_type = typename type::value_type;
	using value_cref = typename type::value_type const&;

	static value_type length(type_cref v) {
		return std::abs(v.x);
	}

	static value_type distance(type_cref v, type_cref r) {
		return std::abs(r.x - v.x);
	}

	static value_type dot(type_cref v, type_cref r) {
		return v.x * r.x;
	}

	static type normalize(type_cref v) {
		return v.x < value_type(0) ? type(-1) : type(1);
	}

	static type faceforward(type_cref n, type_cref i, type_cref ng) {
		return operations::dot(ng, i) < 0 ? type{n} : type{-n.x};
	}

	static type reflect(type_cref i, type_cref n) {
		return i.x - value_type(2) * n.x * operations::dot(n, i);
	}

	static type refract(type_cref i, type_cref n, value_cref eta) {
		value_type const d = operations::dot(n, i);
		value_type const k = value_type(1) - eta * eta * (value_type(1) - d * d);
		return k < value_type(0)
			? type{value_type(0)}
			: type{eta * i.x - (eta * d + std::sqrt(k)) * n.x}
		;
	}
};
/** @endcond */ // INTERNAL

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
/** @name Comparison operators */ /// @{
	/// Equivalence operator.
	template<class T>
	inline bool operator==(TVec1<T> const& x, TVec1<T> const& y) {
		return x.x == y.x;
	}

	/// Non-equivalence operator.
	template<class T>
	inline bool operator!=(TVec1<T> const& x, TVec1<T> const& y) {
		return x.x != y.x;
	}
/// @}
#pragma GCC diagnostic pop

/** @name Arithmetic assignment operators */ /// @{
	/// Add value.
	template<class T, class U>
	inline TVec1<T>& operator+=(TVec1<T>& x, U const& s) {
		x.x += T(s);
		return x;
	}
	/// Add vector.
	template<class T, class U>
	inline TVec1<T>& operator+=(TVec1<T>& x, TVec1<U> const& y) {
		x.x += T(y.x);
		return x;
	}

	/// Subtract value.
	template<class T, class U>
	inline TVec1<T>& operator-=(TVec1<T>& x, U const& s) {
		x.x -= T(s);
		return x;
	}
	/// Subtract vector.
	template<class T, class U>
	inline TVec1<T>& operator-=(TVec1<U>& x, TVec1<U> const& y) {
		x.x -= T(y.x);
		return x;
	}

	/// Multiply by scalar.
	template<class T, class U>
	inline TVec1<T>& operator*=(TVec1<T>& x, U const& s) {
		x.x *= T(s);
		return x;
	}
	/// Multiply by vector (direct component product).
	template<class T, class U>
	inline TVec1<T>& operator*=(TVec1<T>& x, TVec1<U> const& y) {
		x.x *= T(y.x);
		return x;
	}

	/// Divide by value.
	template<class T, class U>
	inline TVec1<T>& operator/=(TVec1<T>& x, U const& s) {
		x.x /= T(s);
		return x;
	}
	/// Divide by vector (direct component quotient).
	template<class T, class U>
	inline TVec1<T>& operator/=(TVec1<T>& x, TVec1<U> const& y) {
		x.x /= T(y.x);
		return x;
	}

	/// Modulo by value.
	template<class T, class U>
	inline TVec1<T>& operator%=(TVec1<T>& x, U const& s) {
		x.x %= T(s);
		return x;
	}
	/// Modulo by vector.
	template<class T, class U>
	inline TVec1<T>& operator%=(TVec1<T>& x, TVec1<U> const& y) {
		x.x %= T(y.x);
		return x;
	}
/// @}

/** @name Bitwise assignment operators */ /// @{
	/// Bitwise-AND value.
	template<class T, class U>
	inline TVec1<T>& operator&=(TVec1<T>& x, U const& s) {
		x.x &= T(s);
		return x;
	}
	/// Bitwise-AND vector.
	template<class T, class U>
	inline TVec1<T>& operator&=(TVec1<T>& x, TVec1<U> const& y) {
		x.x &= T(y.x);
		return x;
	}

	/// Bitwise-OR value.
	template<class T, class U>
	inline TVec1<T>& operator|=(TVec1<T>& x, U const& s) {
		x.x |= T(s);
		return x;
	}
	/// Bitwise-OR vector.
	template<class T, class U>
	inline TVec1<T>& operator|=(TVec1<T>& x, TVec1<U> const& y) {
		x.x |= T(y.x);
		return x;
	}

	/// Bitwise-XOR value.
	template<class T, class U>
	inline TVec1<T>& operator^=(TVec1<T>& x, U const& s) {
		x.x ^= T(s);
		return x;
	}
	/// Bitwise-XOR vector.
	template<class T, class U>
	inline TVec1<T>& operator^=(TVec1<T>& x, TVec1<U> const& y) {
		x.x ^= T(y.x);
		return x;
	}

	/// Bitwise left-shift value.
	template<class T, class U>
	inline TVec1<T>& operator<<=(TVec1<T>& x, U const& s) {
		x.x <<= T(s);
		return x;
	}
	/// Bitwise left-shift vector.
	template<class T, class U>
	inline TVec1<T>& operator<<=(TVec1<T>& x, TVec1<U> const& y) {
		x.x <<= T(y.x);
		return x;
	}

	/// Bitwise right-shift value.
	template<class T, class U>
	inline TVec1<T>& operator>>=(TVec1<T>& x, U const& s) {
		x.x >>= T(s);
		return x;
	}
	/// Bitwise right-shift vector.
	template<class T, class U>
	inline TVec1<T>& operator>>=(TVec1<T>& x, TVec1<U> const& y) {
		x.x >>= T(y.x);
		return x;
	}
/// @}

/** @name Increment and decrement operators */ /// @{
	/// Vector prefix increment.
	template<class T>
	inline TVec1<T>& operator++(TVec1<T>& x) {
		++x.x;
		return x;
	}

	/// Vector prefix decrement.
	template<class T>
	inline TVec1<T>& operator--(TVec1<T>& x) {
		--x.x;
		return x;
	}

	/// Vector postfix increment.
	template<class T>
	inline TVec1<T> operator++(TVec1<T>& x, signed) {
		TVec1<T> c{x};
		++x;
		return c;
	}

	/// Vector postfix decrement.
	template<class T>
	inline TVec1<T> operator--(TVec1<T>& x, signed) {
		TVec1<T> c{x};
		--x;
		return c;
	}
/// @}

/** @name Unary operators */ /// @{
	/// Vector unary plus.
	template<class T>
	inline TVec1<T> operator+(TVec1<T> const& x) {
		return TVec1<T>{x.x};
	}

	/// Vector unary minus.
	template<class T>
	inline TVec1<T> operator-(TVec1<T> const& x) {
		return TVec1<T>{-x.x};
	}

	/// Vector unary bitwise-NOT.
	template<class T>
	inline TVec1<T> operator~(TVec1<T> const& x) {
		return TVec1<T>{~x.x};
	}
/// @}

/** @name Arithmetic operators */ /// @{
	/// Vector right-hand value addition.
	template<class T>
	inline TVec1<T> operator+(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x + y};
	}
	/// Vector left-hand value addition.
	template<class T>
	inline TVec1<T> operator+(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x + y.x};
	}
	/// Vector addition.
	template<class T>
	inline TVec1<T> operator+(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x + y.x};
	}

	/// Vector right-hand value subtraction.
	template<class T>
	inline TVec1<T> operator-(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x - y};
	}
	/// Vector left-hand value subtraction.
	template<class T>
	inline TVec1<T> operator-(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x - y.x};
	}
	/// Vector subtraction.
	template<class T>
	inline TVec1<T> operator-(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x - y.x};
	}

	/// Vector right-hand scalar multiplication.
	template<class T>
	inline TVec1<T> operator*(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x * y};
	}
	/// Vector left-hand scalar multiplication.
	template<class T>
	inline TVec1<T> operator*(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x * y.x};
	}
	/// Vector multiplication (direct component product).
	template<class T>
	inline TVec1<T> operator*(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x * y.x};
	}

	/// Vector right-hand value division.
	template<class T>
	inline TVec1<T> operator/(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x / y};
	}
	/// Vector left-hand value division.
	template<class T>
	inline TVec1<T> operator/(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x / y.x};
	}
	/// Vector division (direct component quotient).
	template<class T>
	inline TVec1<T> operator/(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x / y.x};
	}

	/// Vector right-hand value modulo.
	template<class T>
	inline TVec1<T> operator%(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x % y};
	}
	/// Vector left-hand value modulo.
	template<class T>
	inline TVec1<T> operator%(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x % y.x};
	}
	/// Vector modulo.
	template<class T>
	inline TVec1<T> operator%(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x % y.x};
	}
/// @}

/** @name Bitwise operators */ /// @{
	/// Vector right-hand value bitwise-AND.
	template<class T>
	inline TVec1<T> operator&(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x & y};
	}
	/// Vector left-hand value bitwise-AND.
	template<class T>
	inline TVec1<T> operator&(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x & y.x};
	}
	/// Vector bitwise-AND.
	template<class T>
	inline TVec1<T> operator&(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x & y.x};
	}

	/// Vector right-hand value bitwise-OR.
	template<class T>
	inline TVec1<T> operator|(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x | y};
	}
	/// Vector left-hand value bitwise-OR.
	template<class T>
	inline TVec1<T> operator|(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x | y.x};
	}
	/// Vector bitwise-OR.
	template<class T>
	inline TVec1<T> operator|(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x | y.x};
	}

	/// Vector right-hand value bitwise-XOR.
	template<class T>
	inline TVec1<T> operator^(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x ^ y};
	}
	/// Vector left-hand value bitwise-XOR.
	template<class T>
	inline TVec1<T> operator^(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x ^ y.x};
	}
	/// Vector bitwise-XOR.
	template<class T>
	inline TVec1<T> operator^(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x ^ y.x};
	}

	/// Vector right-hand value bitwise left-shift.
	template<class T>
	inline TVec1<T> operator<<(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x << y};
	}
	/// Vector left-hand value bitwise left-shift.
	template<class T>
	inline TVec1<T> operator<<(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x << y.x};
	}
	/// Vector bitwise left-shift.
	template<class T>
	inline TVec1<T> operator<<(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x << y.x};
	}

	/// Vector right-hand value bitwise right-shift.
	template<class T>
	inline TVec1<T> operator>>(TVec1<T> const& x, T const& y) {
		return TVec1<T>{x.x >> y};
	}
	/// Vector left-hand value bitwise right-shift.
	template<class T>
	inline TVec1<T> operator>>(T const& x, TVec1<T> const& y) {
		return TVec1<T>{x >> y.x};
	}
	/// Vector bitwise right-shift.
	template<class T>
	inline TVec1<T> operator>>(TVec1<T> const& x, TVec1<T> const& y) {
		return TVec1<T>{x.x >> y.x};
	}
/// @}

/** @} */ // end of doc-group lib_core_math_vector_1d

} // namespace togo
} // namespace math
