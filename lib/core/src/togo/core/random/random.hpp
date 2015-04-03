#line 2 "togo/core/random/random.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Random interface.
@ingroup random
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/random/types.hpp>
#include <togo/core/random/random.gen_interface>

namespace togo {

namespace random {

/**
	@addtogroup random
	@{
*/

namespace {

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

/// Iterate RNG and uniformly distribute value to standard
/// distribution: [0.0f, 1.0f].
template<class T = float, class S>
T next_sdist(S& s) {
	TOGO_CONSTRAIN_FLOATING_POINT(T);
	return static_cast<T>(random::next(s)) / static_cast<T>(RNG_MAX);
}

/// Iterate RNG and uniformly distribute value.
template<class S, class T>
T next_udist(S& s, IntUDist<T> const& u) {
	if (u._range == 0) {
		return u._base;
	} else if (u._range == RNG_MAX) {
		return u._base + static_cast<T>(random::next(s));
	} else {
		u64 value;
		do {
			value = random::next(s);
		} while (value >= u._bound);
		return u._base + static_cast<T>(value / u._ratio);
	}
}

/// Iterate RNG and uniformly distribute value.
template<class S, class T>
T next_udist(S& s, RealUDist<T> const& u) {
	return u._base + random::next_sdist<T>(s) * u._range;
}

/** @} */ // end of doc-group random

} // namespace random

/// Construct xorshift64* RNG with seed.
inline XS64M::XS64M(u64 const seed) {
	random::seed(*this, seed);
}

/// Construct xorshift128+ RNG with seed.
inline XS128A::XS128A(u64 const seed) {
	random::seed(*this, seed);
}

/// Construct xorshift1024* RNG with seed.
inline XS1024M::XS1024M(u64 const seed) {
	random::seed(*this, seed);
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

} // namespace togo
