/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file hash.hpp
@brief Hashing utilities.
@ingroup hash
*/

#pragma once

#include <togo/config.hpp>
#include <togo/debug_constraints.hpp>
#include <togo/types.hpp>

#include <am/hash/fnv.hpp>

namespace togo {
namespace hash {

/**
	@addtogroup hash
	@{
*/

// TODO: Build constexpr version of MurmurHash2 (both lengths) and
// use it instead of FNV-1a

namespace {
	static constexpr am::hash::HashLength const
	hash32_length = am::hash::HashLength::HL32,
	hash64_length = am::hash::HashLength::HL64;
}

/** @cond INTERNAL */
TOGO_CONSTRAIN_SAME(hash32, am::detail::hash::fnv_hash_type<hash32_length>)
TOGO_CONSTRAIN_SAME(hash64, am::detail::hash::fnv_hash_type<hash64_length>)
/** @endcond */

/**
	Calculate 32-bit hash.
*/
inline hash32
calc32(
	char const* const data,
	u32 const size
) {
	return
		size != 0
		? am::hash::fnv1a<hash32_length>(data, size)
		: hash32{0}
	;
}

/**
	Calculate 64-bit hash.
*/
inline hash64
calc64(
	char const* const data,
	u32 const size
) {
	return
		size != 0
		? am::hash::fnv1a<hash64_length>(data, size)
		: hash64{0}
	;
}

/**
	32-bit hash literal.
*/
inline constexpr hash32
operator"" _hash32(
	char const* const data,
	std::size_t const size
) {
	return
		size != 0
		? am::hash::fnv1a_c<hash32_length>(data, size)
		: hash32{0}
	;
}

/**
	64-bit hash literal.
*/
inline constexpr hash64
operator"" _hash64(
	char const* const data,
	std::size_t const size
) {
	return
		size != 0
		? am::hash::fnv1a_c<hash64_length>(data, size)
		: hash64{0}
	;
}

/// 32-bit hash identity (hash of nothing).
static constexpr hash32 const
IDENTITY32{0}; // = ""_hash32;

/// 64-bit hash identity (hash of nothing).
static constexpr hash64 const
IDENTITY64{0}; // = ""_hash64;

/** @} */ // end of doc-group hash

} // namespace hash
} // namespace togo
