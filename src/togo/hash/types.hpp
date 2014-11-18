#line 2 "togo/hash/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Hash types.
@ingroup types
@ingroup hash
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/utility/constraints.hpp>

#include <am/hash/common.hpp>
#include <am/hash/fnv.hpp>

namespace togo {

/**
	@addtogroup hash
	@{
*/

namespace hash {

/// Hash value type lengths.
static constexpr auto const
/// 32-bit hash length.
LENGTH32 = am::hash::HashLength::HL32,
/// 64-bit hash length.
LENGTH64 = am::hash::HashLength::HL64;

/// 32-bit hash values.
enum : hash32 {
	/// 32-bit hash identity (hash of nothing).
	IDENTITY32 = 0,
};

/// 64-bit hash values.
enum : hash64 {
	/// 64-bit hash identity (hash of nothing).
	IDENTITY64 = 0,
};

/// Hash value type properties.
///
/// length and identity constants are supplied for hash value types.
template<class H>
struct traits;

/** @cond INTERNAL */
template<> struct traits<hash32> {
	static constexpr auto const length = hash::LENGTH32;
	static constexpr auto const identity = hash::IDENTITY32;
	using impl = am::hash::fnv1a<length>;
};

template<> struct traits<hash64> {
	static constexpr auto const length = hash::LENGTH64;
	static constexpr auto const identity = hash::IDENTITY64;
	using impl = am::hash::fnv1a<length>;
};

TOGO_CONSTRAIN_SAME(hash32, am::detail::hash::fnv_hash_type<hash::LENGTH32>);
TOGO_CONSTRAIN_SAME(hash64, am::detail::hash::fnv_hash_type<hash::LENGTH64>);
/** @endcond */

} // namespace hash

/** @} */ // end of doc-group hash

} // namespace togo
