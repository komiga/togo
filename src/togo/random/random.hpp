#line 2 "togo/random/random.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Random interface.
@ingroup random
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/random/types.hpp>
#include <togo/utility/traits.hpp>
#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>

namespace togo {

/**
	@addtogroup random
	@{
*/

// Public domain xorshift RNGs sourced from:
// http://xorshift.di.unimi.it/

namespace {

enum : u64 {
	XS64M_MULTIPLIER = 0x2545F4914F6CDD1Dull,
	XS1024M_MULTIPLIER = 0x106689D45497FDB5ull,
};

/*template<class T, class = void>
struct rgn_range_sign_fixed;

template<class T>
struct rgn_range_sign_fixed<T, enable_if<!is_signed<T>::value>> {
	static constexpr u64 const MIN = RNG_MIN;
	static constexpr u64 const MAX = RNG_MAX;
};

template<class T>
struct rgn_range_sign_fixed<T, enable_if<is_signed<T>::value>> {
	static constexpr s64 const MIN = RNG_MIN_S;
	static constexpr s64 const MAX = RNG_MAX_S;
};*/

} // anonymous namespace

/// Seed xorshift64* RNG.
inline void rng_seed(XS64M& s, u64 const seed) {
	TOGO_DEBUG_ASSERT(seed != 0, "seed value must be non-zero");
	s._v = seed;
}

/// Iterate xorshift64* RNG.
inline u64 rng_next(XS64M& s) {
	s._v ^= s._v >> 12; // a
	s._v ^= s._v << 25; // b
	s._v ^= s._v >> 27; // c
	return s._v * XS64M_MULTIPLIER;
}

/// Seed xorshift128+ RNG.
void rng_seed(XS128A& s, u64 seed) {
	TOGO_DEBUG_ASSERT(seed != 0, "seed value must be non-zero");
	XS64M seed_rng{seed};
	s._v[0] = rng_next(seed_rng);
	s._v[1] = rng_next(seed_rng);
}

/// Iterate xorshift128+ RNG.
u64 rng_next(XS128A& s) {
	u64 s1 = s._v[0];
	u64 const s0 = s._v[1];
	s._v[0] = s0;
	s1 ^= s1 << 23; // a
	return (s._v[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26)) + s0; // b, c
}

/// Seed xorshift1024* RNG.
void rng_seed(XS1024M& s, u64 const seed) {
	TOGO_DEBUG_ASSERT(seed != 0, "seed value must be non-zero");
	XS64M seed_rng{seed};
	s._i = 0;
	for (unsigned i = 0; i < array_extent(s._v); ++i) {
		s._v[i] = rng_next(seed_rng);
	}
}

/// Iterate xorshift1024* RNG.
u64 rng_next(XS1024M& s) {
	u64 s0 = s._v[s._i];
	u64 s1 = s._v[s._i = (s._i + 1) & 15];
	s1 ^= s1 << 31; // a
	s1 ^= s1 >> 11; // b
	s0 ^= s0 >> 30; // c
	return (s._v[s._i] = s0 ^ s1) * XS1024M_MULTIPLIER;
}

/// Iterate RNG and uniformly distribute value to standard
/// distribution: [0.0f, 1.0f].
template<class T = float, class S>
T rng_next_sdist(S& s) {
	TOGO_CONSTRAIN_FLOATING_POINT(T);
	return static_cast<T>(rng_next(s)) / static_cast<T>(RNG_MAX);
}

/// Iterate RNG and uniformly distribute value.
template<class S, class T>
T rng_next_udist(S& s, IntUDist<T> const& u) {
	if (u._range == 0) {
		return u._base;
	} else if (u._range == RNG_MAX) {
		return u._base + static_cast<T>(rng_next(s));
	} else {
		u64 value;
		do {
			value = rng_next(s);
		} while (value >= u._bound);
		return u._base + static_cast<T>(value / u._ratio);
	}
}

/// Iterate RNG and uniformly distribute value.
template<class S, class T>
T rng_next_udist(S& s, RealUDist<T> const& u) {
	return u._base + rng_next_sdist<T>(s) * u._range;
}

/// Construct xorshift64* RNG with seed.
inline XS64M::XS64M(u64 const seed) {
	rng_seed(*this, seed);
}

/// Construct xorshift128+ RNG with seed.
inline XS128A::XS128A(u64 const seed) {
	rng_seed(*this, seed);
}

/// Construct xorshift1024* RNG with seed.
inline XS1024M::XS1024M(u64 const seed) {
	rng_seed(*this, seed);
}

/// Construct uniform integer distribution from inclusive range.
template<class T>
inline constexpr IntUDist<T>::IntUDist(
	value_type const min,
	value_type const max
)
	: _base(min)
	, _range(static_cast<u64>(max) - static_cast<u64>(min))
	, _ratio(
		_range == RNG_MAX
		? 1
		: RNG_MAX / (_range + 1)
	)
	, _bound(_ratio * (_range + 1))
{}

/// Construct uniform integer distribution from inclusive range.
template<class T>
inline constexpr RealUDist<T>::RealUDist(
	value_type const min,
	value_type const max
)
	: _base(min)
	, _range(max - min)
{}

/** @} */ // end of doc-group random

} // namespace togo
