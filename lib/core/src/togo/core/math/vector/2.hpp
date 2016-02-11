#line 2 "togo/core/math/vector/2.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 2-dimensional vector interface.
@ingroup lib_core_math
@ingroup lib_core_math_vector
@ingroup lib_core_math_vector_2d
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/vector/2_type.hpp>

#include <cmath>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_vector_2d
	@{
*/

/** @cond INTERNAL */
template<class T>
struct TVec2<T>::operations {
	using type = typename TVec2<T>::type;
	using type_cref = type const&;
	using value_type = typename type::value_type;
	using value_cref = typename type::value_type const&;

	static value_type length(type_cref v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	static value_type distance(type_cref v, type_cref r) {
		return operations::length(r - v);
	}

	static value_type dot(type_cref v, type_cref r) {
		return v.x * r.x + v.y * r.y;
	}

	static type normalize(type_cref v) {
		return v * (value_type(1) / std::sqrt(v.x * v.x + v.y * v.y));
	}

	static type faceforward(type_cref n, type_cref i, type_cref ng) {
		return operations::dot(ng, i) < 0 ? n : -n;
	}

	static type reflect(type_cref i, type_cref n) {
		return i - value_type(2) * n * operations::dot(n, i);
	}

	static type refract(type_cref i, type_cref n, value_cref eta) {
		value_type const d = operations::dot(n, i);
		value_type const k = value_type(1) - eta * eta * (value_type(1) - d * d);
		return k < value_type(0)
			? type{value_type(0)}
			: type{eta * i - (eta * d + std::sqrt(k)) * n}
		;
	}
};
/** @endcond */ // INTERNAL

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
/** @name vec2 comparison operators */ /// @{
	/// Equivalence operator.
	template<class T>
	inline bool operator==(TVec2<T> const& x, TVec2<T> const& y) {
		return
			x.x == y.x &&
			x.y == y.y
		;
	}

	/// Non-equivalence operator.
	template<class T>
	inline bool operator!=(TVec2<T> const& x, TVec2<T> const& y) {
		return
			x.x != y.x ||
			x.y != y.y
		;
	}
/// @}
#pragma GCC diagnostic pop

/** @name vec2 arithmetic assignment operators */ /// @{
	/// Add value to all components.
	template<class T, class U>
	inline TVec2<T>& operator+=(TVec2<T>& x, U const& s) {
		x.x += T(s);
		x.y += T(s);
		return x;
	}
	/// Add vector.
	template<class T, class U>
	inline TVec2<T>& operator+=(TVec2<T>& x, TVec2<U> const& y) {
		x.x += T(y.x);
		x.y += T(y.y);
		return x;
	}

	/// Subtract value from all components.
	template<class T, class U>
	inline TVec2<T>& operator-=(TVec2<T>& x, U const& s) {
		x.x -= T(s);
		x.y -= T(s);
		return x;
	}
	/// Subtract vector.
	template<class T, class U>
	inline TVec2<T>& operator-=(TVec2<T>& x, TVec2<U> const& y) {
		x.x -= T(y.x);
		x.y -= T(y.y);
		return x;
	}

	/// Multiply by scalar.
	template<class T, class U>
	inline TVec2<T>& operator*=(TVec2<T>& x, U const& s) {
		x.x *= T(s);
		x.y *= T(s);
		return x;
	}
	/// Multiply by vector (direct component product).
	template<class T, class U>
	inline TVec2<T>& operator*=(TVec2<T>& x, TVec2<U> const& y) {
		x.x *= T(y.x);
		x.y *= T(y.y);
		return x;
	}

	/// Divide all components by value.
	template<class T, class U>
	inline TVec2<T>& operator/=(TVec2<T>& x, U const& s) {
		x.x /= T(s);
		x.y /= T(s);
		return x;
	}
	/// Divide by vector (direct component quotient).
	template<class T, class U>
	inline TVec2<T>& operator/=(TVec2<T>& x, TVec2<U> const& y) {
		x.x /= T(y.x);
		x.y /= T(y.y);
		return x;
	}

	/// Modulo all components by value.
	template<class T, class U>
	inline TVec2<T>& operator%=(TVec2<T>& x, U const& s) {
		x.x %= T(s);
		x.y %= T(s);
		return x;
	}
	/// Modulo by vector.
	template<class T, class U>
	inline TVec2<T>& operator%=(TVec2<T>& x, TVec2<U> const& y) {
		x.x %= T(y.x);
		x.y %= T(y.y);
		return x;
	}
/// @}

/** @name vec2 bitwise assignment operators */ /// @{
	/// Bitwise-AND all components by value.
	template<class T, class U>
	inline TVec2<T>& operator&=(TVec2<T>& x, U const& s) {
		x.x &= T(s);
		x.y &= T(s);
		return x;
	}
	/// Bitwise-AND vector.
	template<class T, class U>
	inline TVec2<T>& operator&=(TVec2<T>& x, TVec2<U> const& y) {
		x.x &= T(y.x);
		x.y &= T(y.y);
		return x;
	}

	/// Bitwise-OR all components by value.
	template<class T, class U>
	inline TVec2<T>& operator|=(TVec2<T>& x, U const& s) {
		x.x |= T(s);
		x.y |= T(s);
		return x;
	}
	/// Bitwise-OR vector.
	template<class T, class U>
	inline TVec2<T>& operator|=(TVec2<T>& x, TVec2<U> const& y) {
		x.x |= T(y.x);
		x.y |= T(y.y);
		return x;
	}

	/// Bitwise-XOR all components by value.
	template<class T, class U>
	inline TVec2<T>& operator^=(TVec2<T>& x, U const& s) {
		x.x ^= T(s);
		x.y ^= T(s);
		return x;
	}
	/// Bitwise-XOR vector.
	template<class T, class U>
	inline TVec2<T>& operator^=(TVec2<T>& x, TVec2<U> const& y) {
		x.x ^= T(y.x);
		x.y ^= T(y.y);
		return x;
	}

	/// Bitwise left-shift all components by value.
	template<class T, class U>
	inline TVec2<T>& operator<<=(TVec2<T>& x, U const& s) {
		x.x <<= T(s);
		x.y <<= T(s);
		return x;
	}
	/// Bitwise left-shift vector.
	template<class T, class U>
	inline TVec2<T>& operator<<=(TVec2<T>& x, TVec2<U> const& y) {
		x.x <<= T(y.x);
		x.y <<= T(y.y);
		return x;
	}

	/// Bitwise right-shift all components by value.
	template<class T, class U>
	inline TVec2<T>& operator>>=(TVec2<T>& x, U const& s) {
		x.x >>= T(s);
		x.y >>= T(s);
		return x;
	}
	/// Bitwise right-shift vector.
	template<class T, class U>
	inline TVec2<T>& operator>>=(TVec2<T>& x, TVec2<U> const& y) {
		x.x >>= T(y.x);
		x.y >>= T(y.y);
		return x;
	}
/// @}

/** @name vec2 increment and decrement operators */ /// @{
	/// Prefix increment.
	template<class T>
	inline TVec2<T>& operator++(TVec2<T>& x) {
		++x.x;
		++x.y;
		return x;
	}

	/// Prefix decrement.
	template<class T>
	inline TVec2<T>& operator--(TVec2<T>& x) {
		--x.x;
		--x.y;
		return x;
	}

	/// Vector postfix increment.
	template<class T>
	inline TVec2<T> operator++(TVec2<T>& x, signed) {
		TVec2<T> c{x};
		++x;
		return c;
	}

	/// Vector postfix decrement.
	template<class T>
	inline TVec2<T> operator--(TVec2<T>& x, signed) {
		TVec2<T> c{x};
		--x;
		return c;
	}
/// @}

/** @name vec2 unary operators */ /// @{
	/// Vector unary plus.
	template<class T>
	inline TVec2<T> operator+(TVec2<T> const& x) {
		return TVec2<T>{x.x, x.y};
	}

	/// Vector unary minus.
	template<class T>
	inline TVec2<T> operator-(TVec2<T> const& x) {
		return TVec2<T>{
			-x.x,
			-x.y
		};
	}

	/// Vector unary bitwise-NOT.
	template<class T>
	inline TVec2<T> operator~(TVec2<T> const& x) {
		return TVec2<T>{
			~x.x,
			~x.y
		};
	}
/// @}

/** @name vec2 arithmetic operators */ /// @{
	/// Vector right-hand value addition (all components).
	template<class T>
	inline TVec2<T> operator+(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x + y,
			x.y + y
		};
	}
	/// Vector left-hand value addition (all components).
	template<class T>
	inline TVec2<T> operator+(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x + y.x,
			x + y.y
		};
	}
	/// Vector addition.
	template<class T>
	inline TVec2<T> operator+(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x + y.x,
			x.y + y.y
		};
	}

	/// Vector right-hand value subtraction (all components).
	template<class T>
	inline TVec2<T> operator-(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x - y,
			x.y - y
		};
	}
	/// Vector left-hand value subtraction (all components).
	template<class T>
	inline TVec2<T> operator-(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x - y.x,
			x - y.y
		};
	}
	/// Vector subtraction.
	template<class T>
	inline TVec2<T> operator-(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x - y.x,
			x.y - y.y
		};
	}

	/// Vector right-hand scalar multiplication (all components).
	template<class T>
	inline TVec2<T> operator*(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x * y,
			x.y * y
		};
	}
	/// Vector left-hand scalar multiplication (all components).
	template<class T>
	inline TVec2<T> operator*(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x * y.x,
			x * y.y
		};
	}
	/// Vector multiplication (direct component product).
	template<class T>
	inline TVec2<T> operator*(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x * y.x,
			x.y * y.y
		};
	}

	/// Vector right-hand value division (all components).
	template<class T>
	inline TVec2<T> operator/(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x / y,
			x.y / y
		};
	}
	/// Vector left-hand value division (all components).
	template<class T>
	inline TVec2<T> operator/(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x / y.x,
			x / y.y
		};
	}
	/// Vector division (direct component quotient).
	template<class T>
	inline TVec2<T> operator/(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x / y.x,
			x.y / y.y
		};
	}

	/// Vector right-hand value modulo (all components).
	template<class T>
	inline TVec2<T> operator%(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x % y,
			x.y % y
		};
	}
	/// Vector left-hand value modulo (all components).
	template<class T>
	inline TVec2<T> operator%(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x % y.x,
			x % y.y
		};
	}
	/// Vector modulo.
	template<class T>
	inline TVec2<T> operator%(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x % y.x,
			x.y % y.y
		};
	}
/// @}

/** @name vec2 bitwise operators */ /// @{
	/// Vector right-hand value bitwise-AND (all components).
	template<class T>
	inline TVec2<T> operator&(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x & y,
			x.y & y
		};
	}
	/// Vector left-hand value bitwise-AND (all components).
	template<class T>
	inline TVec2<T> operator&(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x & y.x,
			x & y.y
		};
	}
	/// Vector bitwise-AND.
	template<class T>
	inline TVec2<T> operator&(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x & y.x,
			x.y & y.y
		};
	}

	/// Vector right-hand value bitwise-OR (all components).
	template<class T>
	inline TVec2<T> operator|(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x | y,
			x.y | y
		};
	}
	/// Vector left-hand value bitwise-OR (all components).
	template<class T>
	inline TVec2<T> operator|(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x | y.x,
			x | y.y
		};
	}
	/// Vector bitwise-OR.
	template<class T>
	inline TVec2<T> operator|(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x | y.x,
			x.y | y.y
		};
	}

	/// Vector right-hand value bitwise-XOR (all components).
	template<class T>
	inline TVec2<T> operator^(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x ^ y,
			x.y ^ y
		};
	}
	/// Vector left-hand value bitwise-XOR (all components).
	template<class T>
	inline TVec2<T> operator^(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x ^ y.x,
			x ^ y.y
		};
	}
	/// Vector bitwise-XOR.
	template<class T>
	inline TVec2<T> operator^(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x ^ y.x,
			x.y ^ y.y
		};
	}

	/// Vector right-hand value bitwise left-shift (all components).
	template<class T>
	inline TVec2<T> operator<<(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x << y,
			x.y << y
		};
	}
	/// Vector left-hand value bitwise left-shift (all components).
	template<class T>
	inline TVec2<T> operator<<(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x << y.x,
			x << y.y
		};
	}
	/// Vector bitwise left-shift.
	template<class T>
	inline TVec2<T> operator<<(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x << y.x,
			x.y << y.y
		};
	}

	/// Vector right-hand value bitwise right-shift (all components).
	template<class T>
	inline TVec2<T> operator>>(TVec2<T> const& x, T const& y) {
		return TVec2<T>{
			x.x >> y,
			x.y >> y
		};
	}
	/// Vector left-hand value bitwise right-shift (all components).
	template<class T>
	inline TVec2<T> operator>>(T const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x >> y.x,
			x >> y.y
		};
	}
	/// Vector bitwise right-shift.
	template<class T>
	inline TVec2<T> operator>>(TVec2<T> const& x, TVec2<T> const& y) {
		return TVec2<T>{
			x.x >> y.x,
			x.y >> y.y
		};
	}
/// @}

/** @} */ // end of doc-group lib_core_math_vector_2d

} // namespace togo
} // namespace math
