
#include <togo/core/log/log.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/random/random.hpp>

#include <limits>
#include <cmath>
//#include <random>

#include <togo/support/test.hpp>

using namespace togo;

template<class T>
using nlim = std::numeric_limits<T>;

template<class T>
void stats(Array<T> const& values, T const min, T const max) {
	using D = long double;

	D sum = 0.0L;
	for (T const x : values) {
		sum += static_cast<D>(x);
	}

	D const mean = sum / static_cast<D>(array::size(values));
	D var = 0.0L;
	for (T const x : values) {
		D const d = static_cast<D>(x) - mean;
		var += d * d;
	}
	D const stddev = std::sqrt(var / static_cast<D>(array::size(values)));
	//TOGO_LOGF("sum = %Lf  mean = %Lf  var = %Lf\n", sum, mean, var);
	TOGO_LOGF(
		"stddev / range = %.6Lf  "
		"stddev = %.6Lg\n",
		stddev / (static_cast<D>(max) - static_cast<D>(min)),
		stddev
	);
}

template<unsigned N, class U, class S>
void test_udist(
	S& s,
	typename U::value_type const min,
	typename U::value_type const max
) {
	using T = typename U::value_type;

	U const udist{min, max};
	//std::uniform_int_distribution<T> udist{min, max};

	Array<T> values{memory::default_allocator()};
	array::resize(values, N);
	unsigned i = 0;
	while (i < N) {
		T const x = random::next_udist(s, udist);
		//T const x = udist(s);
		TOGO_ASSERTE(min <= x && x <= max);
		values[i++] = x;
	}
	stats(values, min, max);
}

template<unsigned N, class T, class S>
void test_sdist(S& s) {
	//std::uniform_real_distribution<T> sdist{0.0f, 1.0f};

	Array<T> values{memory::default_allocator()};
	array::resize(values, N);
	unsigned i = 0;
	while (i < N) {
		T const x = random::next_sdist(s);
		//T const x = sdist(s);
		TOGO_ASSERTE(0.0f <= x && x <= 1.0f);
		values[i++] = x;
	}
	stats(values, T{0.0f}, T{1.0f});
}

template<class S>
void test_rng(u64 const seed) {
	enum : unsigned {
		N = static_cast<unsigned>(1e6),
	};

	S s{seed};
	//std::mt19937_64 s{seed};

	TOGO_LOG("\nIntUDist:\n");
	test_udist<N, IntUDist<s32>>(s, nlim<s32>::min(), nlim<s32>::max());
	test_udist<N, IntUDist<u32>>(s, nlim<u32>::min(), nlim<u32>::max());
	test_udist<N, IntUDist<s64>>(s, nlim<s64>::min(), nlim<s64>::max());
	test_udist<N, IntUDist<u64>>(s, nlim<u64>::min(), nlim<u64>::max());

	TOGO_LOG("\nRealUDist:\n");
	test_udist<N, RealUDist<float>>(s, nlim<float>::min(), nlim<float>::max());
	test_udist<N, RealUDist<double>>(s, nlim<double>::min(), nlim<double>::max());
	test_udist<N, RealUDist<long double>>(s, nlim<double>::min(), nlim<double>::max());

	TOGO_LOG("\nstandard distribution:\n");
	test_sdist<N, float>(s);
	test_sdist<N, double>(s);
	test_sdist<N, long double>(s);
}

signed main() {
	memory_init();

	TOGO_LOGF("unsigned range: [0x%lX, 0x%lX]  [%lu, %lu]\n", RNG_MIN  , RNG_MAX  , RNG_MIN  , RNG_MAX);
	TOGO_LOGF("  signed range: [0x%lX, 0x%lX]  [%ld, %ld]\n", RNG_MIN_S, RNG_MAX_S, RNG_MIN_S, RNG_MAX_S);

	u64 const seed = system::secs_since_epoch();
	TOGO_LOG("\nXS64M:\n");
	test_rng<XS64M>(seed);
	TOGO_LOG("\nXS128A:\n");
	test_rng<XS128A>(seed);
	TOGO_LOG("\nXS1024M:\n");
	test_rng<XS1024M>(seed);

	return 0;
}
