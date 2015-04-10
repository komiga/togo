#line 2 "togo/core/utility/endian.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Endian utilities.
@ingroup lib_core_utility
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/traits.hpp>

#include <type_traits>

#if defined(TOGO_PLATFORM_LINUX)
	#include <byteswap.h>
#endif

#if defined(bswap_16)
	#define TOGO_BSWAP_16(x) bswap_16(x)
#else
	#define TOGO_BSWAP_16(x) (		\
		(((x) & 0xFF00u) >> 8 ) |	\
		(((x) & 0x00FFu) << 8 )		\
	)
#endif

#if defined(bswap_32)
	#define TOGO_BSWAP_32(x) bswap_32(x)
#else
	#define TOGO_BSWAP_32(x) (			\
		(((x) & 0xFF000000u) >> 24) |	\
		(((x) & 0x00FF0000u) >> 8 ) |	\
		(((x) & 0x0000FF00u) << 8 ) |	\
		(((x) & 0x000000FFu) << 24)		\
	)
#endif

#if defined(bswap_64)
	#define TOGO_BSWAP_64(x) bswap_64(x)
#else
	#define TOGO_BSWAP_64(x) (					\
		(((x) & 0xFF00000000000000ull) >> 56) |	\
		(((x) & 0x00FF000000000000ull) >> 40) |	\
		(((x) & 0x0000FF0000000000ull) >> 24) |	\
		(((x) & 0x000000FF00000000ull) >>  8) |	\
		(((x) & 0x00000000FF000000ull) <<  8) |	\
		(((x) & 0x0000000000FF0000ull) << 24) |	\
		(((x) & 0x000000000000FF00ull) << 40) |	\
		(((x) & 0x00000000000000FFull) << 56)	\
	)
#endif

namespace togo {

/**
	@addtogroup lib_core_utility
	@{
*/

/** @cond INTERNAL */
namespace {

// bswap_X() may be implemented with casts
#if defined(TOGO_COMPILER_CLANG) || defined(TOGO_COMPILER_GCC)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

template<class T, class P>
union rb_proxy {
	T value;
	P integer;
};

template<class T, unsigned = sizeof(T)>
struct reverse_bytes_impl;

template<class T>
struct reverse_bytes_impl<T, 2> {
	inline static T f(rb_proxy<T, u16>&& p) {
		p.integer = TOGO_BSWAP_16(p.integer);
		return p.value;
	}
};

template<class T>
struct reverse_bytes_impl<T, 4> {
	inline static T f(rb_proxy<T, u32>&& p) {
		p.integer = TOGO_BSWAP_32(p.integer);
		return p.value;
	}
};

template<class T>
struct reverse_bytes_impl<T, 8> {
	inline static T f(rb_proxy<T, u64>&& p) {
		p.integer = TOGO_BSWAP_64(p.integer);
		return p.value;
	}
};

#if defined(TOGO_COMPILER_CLANG) || defined(TOGO_COMPILER_GCC)
	#pragma GCC diagnostic pop
#endif

} // anonymous namespace
/** @endcond */ // INTERNAL

/** @name Endian utilities */ /// @{

/// Reverse bytes of arithmetic value.
template<class T>
inline T reverse_bytes(T const value) {
	static_assert(
		sizeof(T) > 1 &&
		is_arithmetic<T>::value,
		"T must be an arithmetic type of size 2 <= 2^n <= 8"
	);
	return reverse_bytes_impl<T>::f({value});
}

/// Reverse bytes of arithmetic value if endian differs from the system.
template<class T>
inline T reverse_bytes_if(T value, Endian const endian) {
	return endian == Endian::system
		? value
		: reverse_bytes(value)
	;
}

/// @}

/** @} */ // end of doc-group lib_core_utility

} // namespace togo
