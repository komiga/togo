#line 2 "togo/core/hash/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Hash types.
@ingroup lib_core_types
@ingroup lib_core_hash
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/constraints.hpp>

namespace togo {

/**
	@addtogroup lib_core_hash
	@{
*/

namespace hash {

/// Hash value sizes.
enum Size : unsigned {
	/// 32-bit hash.
	HS32 = 4u,
	/// 64-bit hash.
	HS64 = 8u,
};

/** @cond INTERNAL */
template<hash::Size S>
struct ValueImpl {
	struct type {
		union {
			u8 data[static_cast<unsigned>(S)];
			u32 chunks[static_cast<unsigned>(S) >> 2];
		};
	};
};

template<>
struct ValueImpl<hash::HS32> {
	using type = hash32;
};
template<>
struct ValueImpl<hash::HS64> {
	using type = hash64;
};
/** @endcond */ // INTERNAL

/// Hash value type.
template<hash::Size S>
using Value = typename ValueImpl<S>::type;

/// FNV-1a hasher.
template<hash::Size S>
struct FNV1a {
	static_assert(
		S == hash::HS32 || S == hash::HS64,
		"FNV-1a is only implemented for 32- and 64-bit hash values"
	);

	using Value = hash::Value<S>;

	static constexpr Value const identity = 0;

	Value value;
	unsigned size;

	FNV1a();

	static Value
	calc(
		u8 const* const data,
		unsigned const size
	);

	static constexpr Value
	calc_ce_seq(
		char const* const data,
		unsigned const size,
		unsigned const index,
		Value const value
	);

	static constexpr Value
	calc_ce(
		char const* const data,
		unsigned const size
	);
};

/// Default hasher.
template<hash::Size S>
using Default = hash::FNV1a<S>;

/// Default hasher (32-bit).
using Default32 = Default<hash::HS32>;
/// Default hasher (64-bit).
using Default64 = Default<hash::HS64>;

/// 32-bit hash values.
enum : Default32::Value {
    /// 32-bit hash identity (hash of nothing).
    IDENTITY32 = Default32::identity,
};

/// 64-bit hash values.
enum : Default64::Value {
    /// 64-bit hash identity (hash of nothing).
    IDENTITY64 = Default64::identity,
};

} // namespace hash

/** @} */ // end of doc-group lib_core_hash

} // namespace togo
