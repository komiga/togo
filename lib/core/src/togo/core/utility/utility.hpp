#line 2 "togo/core/utility/utility.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Utilities.
@ingroup lib_core_utility
*/

#pragma once

// igen-source: utility/utility_li.cpp

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/constraints.hpp>
#include <togo/core/lua/types.hpp>

#include <type_traits>

#include <togo/core/utility/utility.gen_interface>

namespace togo {

/**
	@addtogroup lib_core_utility
	@{
*/

/** @name Type utilities */ /// @{

/// Cast unsigned integral to signed integral.
template<class T>
inline constexpr typename std::make_signed<T>::type
signed_cast(T const value) noexcept {
	return static_cast<typename std::make_signed<T>::type>(value);
}

/// Cast signed integral to unsigned integral.
template<class T>
inline constexpr typename std::make_unsigned<T>::type
unsigned_cast(T const value) noexcept {
	return static_cast<typename std::make_unsigned<T>::type>(value);
}

/// Number of elements in bounded array.
template<class T, unsigned N>
inline constexpr unsigned
array_extent(T const (&)[N]) noexcept {
	return N;
}

/// Number of elements in bounded array.
template<class T, class U, unsigned N>
inline constexpr unsigned
array_extent(T const (U::* const)[N]) noexcept {
	return N;
}

/// Get sizeof(T) or 0 if T is empty.
template<class T>
inline constexpr unsigned
sizeof_empty() noexcept {
	return std::is_empty<remove_ref<T>>::value ? 0 : sizeof(T);
}

/// @}

/** @name Misc utilities */ /// @{

/// Make an rvalue reference.
///
/// This is equivalent to std::move(), whose name is totally bonkers.
template<class T>
inline constexpr remove_ref<T>&& rvalue_ref(T&& x) noexcept {
	return static_cast<remove_ref<T>&&>(x);
}

/// Retain value category when passing to another function.
///
/// This is equivalent to std::forward().
template<class T>
inline constexpr T&& forward(remove_ref<T>& x) noexcept {
	return static_cast<T&&>(x);
}

template<class T>
inline constexpr T&& forward(remove_ref<T>&& x) noexcept {
	static_assert(
		!is_lvalue_reference<T>::value,
		"rvalue cannot be forwarded as an lvalue"
	);
	return static_cast<T&&>(x);
}

/// A type as an unevaluated value.
///
/// This is equivalent to std::declval().
template<class T>
inline add_rvalue_ref<T> type_value() noexcept;

/// Swap the values of two references.
template<class T>
inline void swap(T& x, T& y) {
	T temp = x;
	x = y;
	y = temp;
}

/// Less-than comparison operator wrapper.
template<class T>
inline bool less(T const& x, T const& y) {
	return x < y;
}

/// Greater-than comparison operator wrapper.
template<class T>
inline bool greater(T const& x, T const& y) {
	return x > y;
}

/// @}

/** @name Arithmetic utilities */ /// @{

/// Get the lesser of two values.
template<class T>
inline constexpr T min(T const x, T const y) noexcept {
	TOGO_CONSTRAIN_INTEGRAL(T);
	return x < y ? x : y;
}

/// Get the greater of two values.
template<class T>
inline constexpr T max(T const x, T const y) noexcept {
	TOGO_CONSTRAIN_INTEGRAL(T);
	return x > y ? x : y;
}

/// Clamp a value between a minimum and maximum.
template<class T>
inline constexpr T clamp(T const x, T const minimum, T const maximum) noexcept {
	TOGO_CONSTRAIN_INTEGRAL(T);
	return
		  x < minimum
		? minimum
		: x > maximum
		? maximum
		: x
	;
}

/// @}

/** @name Memory utilities */ /// @{

/// Advance pointer by bytes.
///
/// @warning This advances by bytes, not sizeof(T).
template<class T>
inline T* pointer_add(T* p, unsigned const bytes) noexcept {
	return reinterpret_cast<T*>(
		reinterpret_cast<type_if<is_const<T>::value, u8 const*, u8*>>(p) + bytes
	);
}

/// Aligns a pointer by moving it forward.
template<class T>
inline T* pointer_align(T* p, unsigned const align) noexcept {
	if (align) {
		unsigned const m = reinterpret_cast<std::uintptr_t>(p) % align;
		if (m) {
			p = pointer_add(p, align - m);
		}
	}
	return p;
}

/// @}

/** @name Enum utilities */ /// @{

/// Whether an enum value is non-zero.
template<class T>
inline constexpr bool enum_bool(T const value) {
	static_assert(
		std::is_enum<T>::value,
		"T must be an enum"
	);
	return static_cast<typename std::underlying_type<T>::type>(value);
}

/** @cond INTERNAL */
template<class FlagT, class = enable_if<enable_enum_bitwise_ops<FlagT>::value>>
inline constexpr FlagT operator|(FlagT const& x, FlagT const& y) noexcept {
	return static_cast<FlagT>(
		static_cast<unsigned>(x) | static_cast<unsigned>(y)
	);
}

template<class FlagT, class = enable_if<enable_enum_bitwise_ops<FlagT>::value>>
inline constexpr FlagT operator&(FlagT const& x, FlagT const& y) noexcept {
	return static_cast<FlagT>(
		static_cast<unsigned>(x) & static_cast<unsigned>(y)
	);
}

template<class FlagT, class = enable_if<enable_enum_bitwise_ops<FlagT>::value>>
inline constexpr FlagT operator~(FlagT const& x) noexcept {
	return static_cast<FlagT>(
		~static_cast<unsigned>(x)
	);
}

template<class FlagT, class = enable_if<enable_enum_bitwise_ops<FlagT>::value>>
inline constexpr FlagT& operator|=(FlagT& x, FlagT const& y) noexcept {
	return x = static_cast<FlagT>(
		static_cast<unsigned>(x) | static_cast<unsigned>(y)
	);
}

template<class FlagT, class = enable_if<enable_enum_bitwise_ops<FlagT>::value>>
inline constexpr FlagT& operator&=(FlagT& x, FlagT const& y) noexcept {
	return x = static_cast<FlagT>(
		static_cast<unsigned>(x) & static_cast<unsigned>(y)
	);
}
/** @endcond */ // INTERNAL

/// @}

/** @name Collection utilities */ /// @{

/// Get value by index.
template<class T>
inline T& ArrayRef<T>::operator[](unsigned const i) {
	TOGO_DEBUG_ASSERTE(_begin + i < _end);
	return _begin[i];
}

/// Get value by index.
template<class T>
inline T const& ArrayRef<T>::operator[](unsigned const i) const {
	TOGO_DEBUG_ASSERTE(_begin + i < _end);
	return _begin[i];
}

/// Make reference to array.
template<class T>
inline ArrayRef<T> array_ref(unsigned const size, T* const data) {
	return ArrayRef<T>{data, data + size};
}

/// Make reference to array.
template<class T, unsigned N>
inline ArrayRef<T> array_ref(T (&data)[N]) {
	return ArrayRef<T>{data, data + N};
}

/// Make reference to const array.
template<class T>
inline ArrayRef<T const> array_cref(unsigned const size, T const* const data) {
	return ArrayRef<T const>{data, data + size};
}

/// Make reference to const array.
template<class T, unsigned N>
inline ArrayRef<T const> array_cref(T const (&data)[N]) {
	return ArrayRef<T const>{data, data + N};
}

/** @cond INTERNAL */
template<class T>
inline T* begin(ArrayRef<T> const& ar) { return ar._begin; }

template<class T>
inline T const* cbegin(ArrayRef<T> const& ar) { return ar._begin; }

template<class T>
inline T* end(ArrayRef<T> const& ar) { return ar._end; }

template<class T>
inline T const* cend(ArrayRef<T> const& ar) { return ar._end; }
/** @endcond */ // INTERNAL

/// @}

/** @} */ // end of doc-group lib_core_utility

} // namespace togo
