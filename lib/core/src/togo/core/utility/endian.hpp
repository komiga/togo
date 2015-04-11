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
#include <togo/core/utility/constraints.hpp>

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

template<class T, unsigned = sizeof(T)>
struct reverse_bytes_impl;

template<class T>
struct reverse_bytes_impl<T, 1> {
	struct Proxy {
		T value;
	};

	inline static void do_value(Proxy& /*p*/) {}
};

template<class T>
struct reverse_bytes_impl<T, 2> {
	union Proxy {
		T value;
		u16 proxy;
	};

	inline static void do_value(Proxy& p) {
		p.proxy = TOGO_BSWAP_16(p.proxy);
	}
};

template<class T>
struct reverse_bytes_impl<T, 4> {
	union Proxy {
		T value;
		u32 proxy;
	};

	inline static void do_value(Proxy& p) {
		p.proxy = TOGO_BSWAP_32(p.proxy);
	}
};

template<class T>
struct reverse_bytes_impl<T, 8> {
	union Proxy {
		T value;
		u64 proxy;
	};

	inline static void do_value(Proxy& p) {
		p.proxy = TOGO_BSWAP_64(p.proxy);
	}
};

template<class T, unsigned = sizeof(T)>
struct reverse_bytes_array_impl {
	inline static void do_array(ArrayRef<T> const& data) {
		using impl_type = reverse_bytes_impl<T>;
		typename impl_type::Proxy proxy;
		for (auto it = begin(data); it != end(data); ++it) {
			proxy.value = *it;
			impl_type::do_value(proxy);
			*it = proxy.value;
		}
	}
};

template<class T>
struct reverse_bytes_array_impl<T, 1> {
	inline static void do_array(ArrayRef<T> const&) {}
};

#if defined(TOGO_COMPILER_CLANG) || defined(TOGO_COMPILER_GCC)
	#pragma GCC diagnostic pop
#endif

} // anonymous namespace
/** @endcond */ // INTERNAL

/** @name Endian utilities */ /// @{

/// Reverse bytes of arithmetic value.
template<class T>
inline void reverse_bytes(T& value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	using impl_type = reverse_bytes_impl<T>;
	typename impl_type::Proxy proxy{value};
	impl_type::do_value(proxy);
	value = proxy.value;
}

/// Reverse bytes of arithmetic value if endian differs from the system.
template<class T>
inline void reverse_bytes_if(T& value, Endian const endian) {
	if (endian != Endian::system) {
		reverse_bytes(value);
	}
}

/// Reverse bytes of arithmetic value.
template<class T>
inline T reverse_bytes_copy(T value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	using impl_type = reverse_bytes_impl<T>;
	typename impl_type::Proxy proxy{value};
	impl_type::do_value(proxy);
	return proxy.value;
}

/// Reverse bytes of arithmetic value if endian differs from the system.
template<class T>
inline T reverse_bytes_copy_if(T value, Endian const endian) {
	if (endian != Endian::system) {
		reverse_bytes(value);
	}
	return value;
}

/// Reverse bytes of arithmetic values in array.
template<class T>
inline void reverse_bytes(ArrayRef<T> const& data) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	reverse_bytes_array_impl<T>::do_array(data);
}

/// Reverse bytes of arithmetic values in array if endian differs from the
/// system.
template<class T>
inline void reverse_bytes_if(ArrayRef<T> const& value, Endian const endian) {
	if (endian != Endian::system) {
		reverse_bytes(value);
	}
}

/// @}

/** @} */ // end of doc-group lib_core_utility

} // namespace togo
