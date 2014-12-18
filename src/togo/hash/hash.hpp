#line 2 "togo/hash/hash.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Hash interface.
@ingroup hash
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/string/string.hpp>
#include <togo/hash/types.hpp>

#include <am/hash/fnv.hpp>

// TODO: Build constexpr version of MurmurHash2 or MurmurHash3
// (both lengths) and use it instead of FNV-1a

namespace togo {

namespace hash {

/**
	@addtogroup hash
	@{
*/

/// Calculate H-bit hash.
template<class H>
inline H calc_generic(
	char const* const data,
	unsigned const size
) {
	return
		size != 0
		? am::hash::calc<typename hash::traits<H>::impl>(data, size)
		: hash::traits<H>::identity
	;
}

/// Calculate H-bit hash from string reference.
template<class H>
inline H calc_generic(StringRef const& ref) {
	return hash::calc_generic<H>(ref.data, ref.size);
}

/// Calculate H-bit hash (constexpr).
template<class H>
inline constexpr H calc_generic_ce(
	char const* const data,
	unsigned const size
) {
	return
		size != 0
		? am::hash::calc_ce<typename hash::traits<H>::impl>(data, size)
		: hash::traits<H>::identity
	;
}

/// Calculate H-bit hash from string reference (constexpr).
template<class H>
inline constexpr H calc_generic_ce(StringRef const& ref) {
	return hash::calc_generic_ce<H>(ref.data, ref.size);
}

/// Calculate 32-bit hash.
inline hash32 calc32(
	char const* const data,
	unsigned const size
) {
	return hash::calc_generic<hash32>(data, size);
}

/// Calculate 32-bit hash from string reference.
inline hash32 calc32(StringRef const& ref) {
	return hash::calc_generic<hash32>(ref.data, ref.size);
}

/// Calculate 32-bit hash  (constexpr).
inline constexpr hash32 calc32_ce(
	char const* const data,
	unsigned const size
) {
	return hash::calc_generic_ce<hash32>(data, size);
}

/// Calculate 32-bit hash from string reference  (constexpr).
inline constexpr hash32 calc32_ce(StringRef const& ref) {
	return hash::calc_generic_ce<hash32>(ref.data, ref.size);
}

/// Calculate 64-bit hash.
inline hash64 calc64(
	char const* const data,
	unsigned const size
) {
	return hash::calc_generic<hash64>(data, size);
}

/// Calculate 64-bit hash from string reference.
inline hash64 calc64(StringRef const& ref) {
	return hash::calc_generic<hash64>(ref.data, ref.size);
}

/// Calculate 64-bit hash (constexpr).
inline constexpr hash64 calc64_ce(
	char const* const data,
	unsigned const size
) {
	return hash::calc_generic_ce<hash64>(data, size);
}

/// Calculate 64-bit hash from string reference (constexpr).
inline constexpr hash64 calc64_ce(StringRef const& ref) {
	return hash::calc_generic_ce<hash64>(ref.data, ref.size);
}

/** @} */ // end of doc-group hash

} // namespace hash

/// 32-bit hash literal.
inline constexpr hash32
operator"" _hash32(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// 64-bit hash literal.
inline constexpr hash64
operator"" _hash64(
	char const* const data,
	std::size_t const size
) {
	return hash::calc64_ce(data, size);
}

} // namespace togo
