#line 2 "togo/random/random.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/random/types.hpp>
#include <togo/random/random.hpp>

namespace togo {

enum : u64 {
	XS64M_MULTIPLIER = 0x2545F4914F6CDD1Dull,
	XS1024M_MULTIPLIER = 0x106689D45497FDB5ull,
};

/// Seed xorshift64* RNG.
inline void random::seed(XS64M& s, u64 const seed) {
	TOGO_DEBUG_ASSERT(seed != 0, "seed value must be non-zero");
	s._v = seed;
}

/// Iterate xorshift64* RNG.
inline u64 random::next(XS64M& s) {
	s._v ^= s._v >> 12; // a
	s._v ^= s._v << 25; // b
	s._v ^= s._v >> 27; // c
	return s._v * XS64M_MULTIPLIER;
}

/// Seed xorshift128+ RNG.
void random::seed(XS128A& s, u64 seed) {
	TOGO_DEBUG_ASSERT(seed != 0, "seed value must be non-zero");
	XS64M seed_rng{seed};
	s._v[0] = random::next(seed_rng);
	s._v[1] = random::next(seed_rng);
}

/// Iterate xorshift128+ RNG.
u64 random::next(XS128A& s) {
	u64 s1 = s._v[0];
	u64 const s0 = s._v[1];
	s._v[0] = s0;
	s1 ^= s1 << 23; // a
	return (s._v[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26)) + s0; // b, c
}

/// Seed xorshift1024* RNG.
void random::seed(XS1024M& s, u64 const seed) {
	TOGO_DEBUG_ASSERT(seed != 0, "seed value must be non-zero");
	XS64M seed_rng{seed};
	s._i = 0;
	for (unsigned i = 0; i < array_extent(s._v); ++i) {
		s._v[i] = random::next(seed_rng);
	}
}

/// Iterate xorshift1024* RNG.
u64 random::next(XS1024M& s) {
	u64 s0 = s._v[s._i];
	u64 s1 = s._v[s._i = (s._i + 1) & 15];
	s1 ^= s1 << 31; // a
	s1 ^= s1 >> 11; // b
	s0 ^= s0 >> 30; // c
	return (s._v[s._i] = s0 ^ s1) * XS1024M_MULTIPLIER;
}

} // namespace togo
