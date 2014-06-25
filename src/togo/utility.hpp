#line 2 "togo/utility.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file utility.hpp
@brief Utilities.
@ingroup utility
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
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

/// @}

/** @name Arithmetic utilities */ /// @{

/// Get the smallest of two values.
template<class T>
inline T
min(T const x, T const y) noexcept {
	TOGO_CONSTRAIN_INTEGRAL(T)
	TOGO_CONSTRAIN_ARITHMETIC(T)
	return x < y ? x : y;
}

/// Get the largest of two values.
template<class T>
inline T
max(T const x, T const y) noexcept {
	TOGO_CONSTRAIN_INTEGRAL(T)
	TOGO_CONSTRAIN_ARITHMETIC(T)
	return x > y ? x : y;
}

/// @}

/** @name Memory utilities */ /// @{

/// Advance pointer by bytes.
/// @warning This advances by bytes, not sizeof(T).
template<class T>
inline T* pointer_add(T* p, u32 const bytes) noexcept {
	return static_cast<T*>(static_cast<char*>(p) + bytes);
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

/** @} */ // end of doc-group utility

} // namespace togo
