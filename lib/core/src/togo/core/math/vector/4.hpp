#line 2 "togo/core/math/vector/4.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief 4-dimensional vector interface.
@ingroup lib_core_math
@ingroup lib_core_math_vector
@ingroup lib_core_math_vector_4d
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>
#include <togo/core/math/vector/4_type.hpp>

#include <cmath>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_vector_4d
	@{
*/

/** @cond INTERNAL */
template<class T>
struct TVec4<T>::operations {
	using type = typename TVec4<T>::type;
	using type_cref = type const&;
	using value_type = typename type::value_type;
	using value_cref = typename type::value_type const&;

	static value_type length(type_cref v) {
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	static value_type distance(type_cref v, type_cref r) {
		return operations::length(r - v);
	}

	static value_type dot(type_cref v, type_cref r) {
		return v.x * r.x + v.y * r.y + v.z * r.z + v.w * r.w;
	}

	static type normalize(type_cref v) {
		return v * (value_type(1) / std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));
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
/** @name vec4 comparison operators */ /// @{
	/// Equivalence operator.
	template<class T>
	inline bool operator==(TVec4<T> const& x, TVec4<T> const& y) {
		return
			x.x == y.x &&
			x.y == y.y &&
			x.z == y.z &&
			x.w == y.w
		;
	}

	/// Non-equivalence operator.
	template<class T>
	inline bool operator!=(TVec4<T> const& x, TVec4<T> const& y) {
		return
			x.x != y.x ||
			x.y != y.y ||
			x.z != y.z ||
			x.w != y.w
		;
	}
/// @}
#pragma GCC diagnostic pop

/** @name vec4 arithmetic assignment operators */ /// @{
	/// Add value to all components.
	template<class T, class U>
	inline TVec4<T>& operator+=(TVec4<T>& x, U const& s) {
		x.x += T(s);
		x.y += T(s);
		x.z += T(s);
		x.w += T(s);
		return x;
	}
	/// Add vector.
	template<class T, class U>
	inline TVec4<T>& operator+=(TVec4<T>& x, TVec4<U> const& y) {
		x.x += T(y.x);
		x.y += T(y.y);
		x.z += T(y.z);
		x.w += T(y.w);
		return x;
	}

	/// Subtract value from all components.
	template<class T, class U>
	inline TVec4<T>& operator-=(TVec4<T>& x, U const& s) {
		x.x -= T(s);
		x.y -= T(s);
		x.z -= T(s);
		x.w -= T(s);
		return x;
	}
	/// Subtract vector.
	template<class T, class U>
	inline TVec4<T>& operator-=(TVec4<T>& x, TVec4<U> const& y) {
		x.x -= T(y.x);
		x.y -= T(y.y);
		x.z -= T(y.z);
		x.w -= T(y.w);
		return x;
	}

	/// Multiply by scalar.
	template<class T, class U>
	inline TVec4<T>& operator*=(TVec4<T>& x, U const& s) {
		x.x *= T(s);
		x.y *= T(s);
		x.z *= T(s);
		x.w *= T(s);
		return x;
	}
	/// Multiply by vector (direct component product).
	template<class T, class U>
	inline TVec4<T>& operator*=(TVec4<T>& x, TVec4<U> const& y) {
		x.x *= T(y.x);
		x.y *= T(y.y);
		x.z *= T(y.z);
		x.w *= T(y.w);
		return x;
	}

	/// Divide all components by value.
	template<class T, class U>
	inline TVec4<T>& operator/=(TVec4<T>& x, U const& s) {
		x.x /= T(s);
		x.y /= T(s);
		x.z /= T(s);
		x.w /= T(s);
		return x;
	}
	/// Divide by vector (direct component quotient).
	template<class T, class U>
	inline TVec4<T>& operator/=(TVec4<T>& x, TVec4<U> const& y) {
		x.x /= T(y.x);
		x.y /= T(y.y);
		x.z /= T(y.z);
		x.w /= T(y.w);
		return x;
	}

	/// Modulo all components by value.
	template<class T, class U>
	inline TVec4<T>& operator%=(TVec4<T>& x, U const& s) {
		x.x %= T(s);
		x.y %= T(s);
		x.z %= T(s);
		x.w %= T(s);
		return x;
	}
	/// Modulo by vector.
	template<class T, class U>
	inline TVec4<T>& operator%=(TVec4<T>& x, TVec4<U> const& y) {
		x.x %= T(y.x);
		x.y %= T(y.y);
		x.z %= T(y.z);
		x.w %= T(y.w);
		return x;
	}
/// @}

/** @name vec4 bitwise assignment operators */ /// @{
	/// Bitwise-AND all components by value.
	template<class T, class U>
	inline TVec4<T>& operator&=(TVec4<T>& x, U const& s) {
		x.x &= T(s);
		x.y &= T(s);
		x.z &= T(s);
		x.w &= T(s);
		return x;
	}
	/// Bitwise-AND vector.
	template<class T, class U>
	inline TVec4<T>& operator&=(TVec4<T>& x, TVec4<U> const& y) {
		x.x &= T(y.x);
		x.y &= T(y.y);
		x.z &= T(y.z);
		x.w &= T(y.w);
		return x;
	}

	/// Bitwise-OR all components by value.
	template<class T, class U>
	inline TVec4<T>& operator|=(TVec4<T>& x, U const& s) {
		x.x |= T(s);
		x.y |= T(s);
		x.z |= T(s);
		x.w |= T(s);
		return x;
	}
	/// Bitwise-OR vector.
	template<class T, class U>
	inline TVec4<T>& operator|=(TVec4<T>& x, TVec4<U> const& y) {
		x.x |= T(y.x);
		x.y |= T(y.y);
		x.z |= T(y.z);
		x.w |= T(y.w);
		return x;
	}

	/// Bitwise-XOR all components by value.
	template<class T, class U>
	inline TVec4<T>& operator^=(TVec4<T>& x, U const& s) {
		x.x ^= T(s);
		x.y ^= T(s);
		x.z ^= T(s);
		x.w ^= T(s);
		return x;
	}
	/// Bitwise-XOR vector.
	template<class T, class U>
	inline TVec4<T>& operator^=(TVec4<T>& x, TVec4<U> const& y) {
		x.x ^= T(y.x);
		x.y ^= T(y.y);
		x.z ^= T(y.z);
		x.w ^= T(y.w);
		return x;
	}

	/// Bitwise left-shift all components by value.
	template<class T, class U>
	inline TVec4<T>& operator<<=(TVec4<T>& x, U const& s) {
		x.x <<= T(s);
		x.y <<= T(s);
		x.z <<= T(s);
		x.w <<= T(s);
		return x;
	}
	/// Bitwise left-shift vector.
	template<class T, class U>
	inline TVec4<T>& operator<<=(TVec4<T>& x, TVec4<U> const& y) {
		x.x <<= T(y.x);
		x.y <<= T(y.y);
		x.z <<= T(y.z);
		x.w <<= T(y.w);
		return x;
	}

	/// Bitwise right-shift all components by value.
	template<class T, class U>
	inline TVec4<T>& operator>>=(TVec4<T>& x, U const& s) {
		x.x >>= T(s);
		x.y >>= T(s);
		x.z >>= T(s);
		x.w >>= T(s);
		return x;
	}
	/// Bitwise right-shift vector.
	template<class T, class U>
	inline TVec4<T>& operator>>=(TVec4<T>& x, TVec4<U> const& y) {
		x.x >>= T(y.x);
		x.y >>= T(y.y);
		x.z >>= T(y.z);
		x.w >>= T(y.w);
		return x;
	}
/// @}

/** @name vec4 increment and decrement operators */ /// @{
	/// Prefix increment.
	template<class T>
	inline TVec4<T>&
	operator++(
		TVec4<T>& x
	) {
		++x.x;
		++x.y;
		++x.z;
		++x.w;
		return x;
	}

	/// Prefix decrement.
	template<class T>
	inline TVec4<T>&
	operator--(
		TVec4<T>& x
	) {
		--x.x;
		--x.y;
		--x.z;
		--x.w;
		return x;
	}

	/// Vector postfix increment.
	template<class T>
	inline TVec4<T> operator++(TVec4<T>& x, signed) {
		TVec4<T> c{x};
		++x;
		return c;
	}

	/// Vector postfix decrement.
	template<class T>
	inline TVec4<T> operator--(TVec4<T>& x, signed) {
		TVec4<T> c{x};
		--x;
		return c;
	}
/// @}

/** @name vec4 unary operators */ /// @{
	/// Vector unary plus.
	template<class T>
	inline TVec4<T> operator+(TVec4<T> const& x) {
		return TVec4<T>{x.x, x.y, x.z, x.w};
	}

	/// Vector unary minus.
	template<class T>
	inline TVec4<T> operator-(TVec4<T> const& x) {
		return TVec4<T>{
			-x.x,
			-x.y,
			-x.z,
			-x.w
		};
	}

	/// Vector unary bitwise-NOT.
	template<class T>
	inline TVec4<T> operator~(TVec4<T> const& x) {
		return TVec4<T>{
			~x.x,
			~x.y,
			~x.z,
			~x.w
		};
	}
/// @}

/** @name vec4 arithmetic operators */ /// @{
	/// Vector right-hand value addition (all components).
	template<class T>
	inline TVec4<T> operator+(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x + y,
			x.y + y,
			x.z + y,
			x.w + y
		};
	}
	/// Vector left-hand value addition (all components).
	template<class T>
	inline TVec4<T> operator+(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x + y.x,
			x + y.y,
			x + y.z,
			x + y.w
		};
	}
	/// Vector addition.
	template<class T>
	inline TVec4<T> operator+(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x + y.x,
			x.y + y.y,
			x.z + y.z,
			x.w + y.w
		};
	}

	/// Vector right-hand value subtraction (all components).
	template<class T>
	inline TVec4<T> operator-(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x - y,
			x.y - y,
			x.z - y,
			x.w - y
		};
	}
	/// Vector left-hand value subtraction (all components).
	template<class T>
	inline TVec4<T> operator-(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x - y.x,
			x - y.y,
			x - y.z,
			x - y.w
		};
	}
	/// Vector subtraction.
	template<class T>
	inline TVec4<T> operator-(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x - y.x,
			x.y - y.y,
			x.z - y.z,
			x.w - y.w
		};
	}

	/// Vector right-hand scalar multiplication (all components).
	template<class T>
	inline TVec4<T> operator*(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x * y,
			x.y * y,
			x.z * y,
			x.w * y
		};
	}
	/// Vector left-hand scalar multiplication (all components).
	template<class T>
	inline TVec4<T> operator*(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x * y.x,
			x * y.y,
			x * y.z,
			x * y.w
		};
	}
	/// Vector multiplication (direct component product).
	template<class T>
	inline TVec4<T> operator*(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x * y.x,
			x.y * y.y,
			x.z * y.z,
			x.w * y.w
		};
	}

	/// Vector right-hand value division (all components).
	template<class T>
	inline TVec4<T> operator/(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x / y,
			x.y / y,
			x.z / y,
			x.w / y
		};
	}
	/// Vector left-hand value division (all components).
	template<class T>
	inline TVec4<T> operator/(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x / y.x,
			x / y.y,
			x / y.z,
			x / y.w
		};
	}
	/// Vector division (direct component quotient).
	template<class T>
	inline TVec4<T> operator/(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x / y.x,
			x.y / y.y,
			x.z / y.z,
			x.w / y.w
		};
	}

	/// Vector right-hand value modulo (all components).
	template<class T>
	inline TVec4<T> operator%(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x % y,
			x.y % y,
			x.z % y,
			x.w % y
		};
	}
	/// Vector left-hand value modulo (all components).
	template<class T>
	inline TVec4<T> operator%(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x % y.x,
			x % y.y,
			x % y.z,
			x % y.w
		};
	}
	/// Vector modulo.
	template<class T>
	inline TVec4<T> operator%(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x % y.x,
			x.y % y.y,
			x.z % y.z,
			x.w % y.w
		};
	}
/// @}

/** @name vec4 bitwise operators */ /// @{
	/// Vector right-hand value bitwise-AND (all components).
	template<class T>
	inline TVec4<T> operator&(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x & y,
			x.y & y,
			x.z & y,
			x.w & y
		};
	}
	/// Vector left-hand value bitwise-AND (all components).
	template<class T>
	inline TVec4<T> operator&(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x & y.x,
			x & y.y,
			x & y.z,
			x & y.w
		};
	}
	/// Vector bitwise-AND.
	template<class T>
	inline TVec4<T> operator&(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x & y.x,
			x.y & y.y,
			x.z & y.z,
			x.w & y.w
		};
	}

	/// Vector right-hand value bitwise-OR (all components).
	template<class T>
	inline TVec4<T> operator|(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x | y,
			x.y | y,
			x.z | y,
			x.w | y
		};
	}
	/// Vector left-hand value bitwise-OR (all components).
	template<class T>
	inline TVec4<T> operator|(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x | y.x,
			x | y.y,
			x | y.z,
			x | y.w
		};
	}
	/// Vector bitwise-OR.
	template<class T>
	inline TVec4<T> operator|(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x | y.x,
			x.y | y.y,
			x.z | y.z,
			x.w | y.w
		};
	}

	/// Vector right-hand value bitwise-XOR (all components).
	template<class T>
	inline TVec4<T> operator^(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x ^ y,
			x.y ^ y,
			x.z ^ y,
			x.w ^ y
		};
	}
	/// Vector left-hand value bitwise-XOR (all components).
	template<class T>
	inline TVec4<T> operator^(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x ^ y.x,
			x ^ y.y,
			x ^ y.z,
			x ^ y.w
		};
	}
	/// Vector bitwise-XOR.
	template<class T>
	inline TVec4<T> operator^(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x ^ y.x,
			x.y ^ y.y,
			x.z ^ y.z,
			x.w ^ y.w
		};
	}

	/// Vector right-hand value bitwise left-shift (all components).
	template<class T>
	inline TVec4<T> operator<<(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x << y,
			x.y << y,
			x.z << y,
			x.w << y
		};
	}
	/// Vector left-hand value bitwise left-shift (all components).
	template<class T>
	inline TVec4<T> operator<<(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x << y.x,
			x << y.y,
			x << y.z,
			x << y.w
		};
	}
	/// Vector bitwise left-shift.
	template<class T>
	inline TVec4<T> operator<<(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x << y.x,
			x.y << y.y,
			x.z << y.z,
			x.w << y.w
		};
	}

	/// Vector right-hand value bitwise right-shift (all components).
	template<class T>
	inline TVec4<T> operator>>(TVec4<T> const& x, T const& y) {
		return TVec4<T>{
			x.x >> y,
			x.y >> y,
			x.z >> y,
			x.w >> y
		};
	}
	/// Vector left-hand value bitwise right-shift (all components).
	template<class T>
	inline TVec4<T> operator>>(T const& x, TVec4<T> const& y ) {
		return TVec4<T>{
			x >> y.x,
			x >> y.y,
			x >> y.z,
			x >> y.w
		};
	}
	/// Vector bitwise right-shift.
	template<class T>
	inline TVec4<T> operator>>(TVec4<T> const& x, TVec4<T> const& y) {
		return TVec4<T>{
			x.x >> y.x,
			x.y >> y.y,
			x.z >> y.z,
			x.w >> y.w
		};
	}
/// @}

/** @} */ // end of doc-group lib_core_math_vector_4d

} // namespace togo
} // namespace math
