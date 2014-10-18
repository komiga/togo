#line 2 "togo/utility.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Utilities.
@ingroup utility
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/traits.hpp>
#include <togo/debug_constraints.hpp>

#include <type_traits>

namespace togo {

/**
	@addtogroup utility
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

/// Get number of elements in bounded array.
template<class T, unsigned N>
inline constexpr unsigned
array_extent(T const (&)[N]) noexcept {
	return N;
}

/// Get number of elements in bounded array.
template<class T, class U, unsigned N>
inline constexpr unsigned
array_extent(T const (U::* const)[N]) noexcept {
	return N;
}

/// Get sizeof type or 0 if the type is empty.
template<class T>
inline constexpr unsigned
sizeof_empty() noexcept {
	return std::is_empty<T>::value ? 0 : sizeof(T);
}

/// @}

/** @name Misc utilities */ /// @{

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

/// Get the smallest of two values.
template<class T>
inline constexpr T min(T const x, T const y) noexcept {
	TOGO_CONSTRAIN_INTEGRAL(T);
	return x < y ? x : y;
}

/// Get the largest of two values.
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
/// @warning This advances by bytes, not sizeof(T).
template<class T>
inline T* pointer_add(T* p, u32 const bytes) noexcept {
	return reinterpret_cast<T*>(reinterpret_cast<char*>(p) + bytes);
}

/// Aligns a pointer by moving it forward.
template<class T>
inline T* pointer_align(T* p, u32 const align) noexcept {
	u32 const m = reinterpret_cast<std::uintptr_t>(p) % align;
	if (m) {
		p = pointer_add(p, align - m);
	}
	return p;
}

/// @}

/** @name Enum utilities */ /// @{

/// Return true whether an enum value is non-zero.
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

/** @} */ // end of doc-group utility

} // namespace togo
