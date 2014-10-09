
#include <togo/log.hpp>
#include <togo/memory.hpp>
#include <togo/array.hpp>
#include <togo/algorithm.hpp>

#include "../common/helpers.hpp"

#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include <cstring>

using namespace togo;

using namespace std::chrono;
using hrc = std::chrono::high_resolution_clock;
using ms_type = milliseconds::rep;

template<class K>
struct Item {
	K key;
	unsigned value;
};

template<class K>
struct ItemKeyFunc {
	inline K operator()(Item<K> const& item) const noexcept {
		return item.key;
	}
};

// Less-than comparator
template<class K>
struct ItemKeyLess {
	inline bool operator()(Item<K> const& x, Item<K> const& y) const noexcept {
		return x.key < y.key;
	}
};

template<class K, class R>
inline void fill_items(Array<Item<K>>& items, unsigned const seed) {
	R rng{seed};
	std::uniform_int_distribution<K> udist_key{};
	std::uniform_int_distribution<unsigned> udist_value{};
	for (unsigned i = 0; i < array::size(items); ++i) {
		items[i] = {
			udist_key(rng),
			udist_value(rng)
		};
	}
}

template<class K, class B, class R>
Array<Item<K>>& test_sort_radix(
	Array<Item<K>>& items,
	Array<Item<K>>& items_swap,
	unsigned const num,
	unsigned const seed,
	ms_type& time
) {
	array::resize(items, num);
	array::resize(items_swap, num);
	fill_items<K, R>(items, seed);

	auto const start = hrc::now();
	auto ptr_items = array::begin(items);
	auto ptr_items_swap = array::begin(items_swap);
	// Observation: function object is faster
	bool const swapped = sort_radix_generic<Item<K>, K, B>(
		ptr_items, ptr_items_swap, num, ItemKeyFunc<K>{}
	);
	time = duration_cast<milliseconds>(hrc::now() - start).count();
	if (swapped) {
		return items_swap;
	} else {
		return items;
	}
}

template<class K, class R>
void test_sort_stdlib(
	Array<Item<K>>& items,
	unsigned const num,
	unsigned const seed,
	ms_type& time
) {
	array::resize(items, num);
	fill_items<K, R>(items, seed);

	auto const start = hrc::now();
	std::stable_sort(array::begin(items), array::end(items), ItemKeyLess<K>{});
	time = duration_cast<milliseconds>(hrc::now() - start).count();
}

template<class K>
void validate(Array<Item<K>> const& items) {
	auto prev_key = items[0].key;
	for (auto const& item : items) {
		TOGO_ASSERTE(prev_key <= item.key);
		prev_key = item.key;
	}
}

template<class K>
void check_equal(Array<Item<K>> const& x, Array<Item<K>> const& y) {
	auto const size = array::size(x);
	TOGO_ASSERTE(size == array::size(y));
	for (unsigned i = 0; i < size; ++i) {
		TOGO_ASSERTE(
			x[i].key == y[i].key &&
			x[i].value == y[i].value
		);
	}
}

template<class K, class B, class R>
void test_and_validate(unsigned const num, unsigned const seed) {
	ms_type time_radix{}, time_stdlib{};
	Array<Item<K>> items_radix{memory::default_allocator()};
	Array<Item<K>> items_radix_swap{memory::default_allocator()};
	Array<Item<K>> items_stdlib{memory::default_allocator()};
	auto const& items_radix_actual = test_sort_radix<K, B, R>(
		items_radix, items_radix_swap, num, seed, time_radix
	);
	test_sort_stdlib<K, R>(
		items_stdlib, num, seed, time_stdlib
	);
	TOGO_LOGF(
		"times: %6lld / %-6lld  num = %-10u  seed = %-12u  "
		"key size = %zu  bin value size = %zu\n",
		time_radix, time_stdlib, num, seed,
		sizeof(K), sizeof(B)
	);
	validate(items_radix_actual);
	validate(items_stdlib);
	check_equal(items_radix_actual, items_stdlib);
}

struct Run {
	unsigned num;
	unsigned seed;
	ms_type time;
};

float print_runs(Array<Run> const& runs) {
	float prev = 0.0f;
	float ratio_accum = 0.0f;
	for (unsigned i = 0; i < array::size(runs); ++i) {
		Run const& run = runs[i];
		float const ratio = static_cast<float>(run.time) / prev;
		prev = static_cast<float>(run.time);
		if (i > 0) {
			ratio_accum += ratio;
		}
		TOGO_LOGF(
			"num = %-10u  time = %-8lld  "
			"ratio = %-6.03f  log2(ratio) = %-6.03f  seed = %u\n",
			run.num,
			run.time,
			ratio,
			std::log2(ratio),
			run.seed
		);
	}
	ratio_accum /= array::size(runs) - 1;
	TOGO_LOGF("average ratio: %-6.03f\n", ratio_accum);
	return ratio_accum;
}

template<class K, class B, class IR, class SR>
void test_growth(
	unsigned const num_runs,
	unsigned num,
	SR& srng
) {
	Array<Item<K>> items{memory::default_allocator()};
	Array<Item<K>> items_swap{memory::default_allocator()};

	Array<Run> runs_radix{memory::default_allocator()};
	array::resize(runs_radix, num_runs);
	for (Run& run : runs_radix) {
		run.num = num;
		run.seed = srng();
		num <<= 1;
	}

	for (Run& run : runs_radix) {
		test_sort_radix<K, B, IR>(
			items, items_swap, run.num, run.seed, run.time
		);
	}
	TOGO_LOGF("radix sort (%u):\n", num_runs);
	float const ratio_avg_radix = print_runs(runs_radix);

	TOGO_LOG("\n");
	Array<Run> runs_stdlib{memory::default_allocator()};
	array::resize(runs_stdlib, num_runs);
	std::memcpy(array::begin(runs_stdlib), array::begin(runs_radix), num_runs * sizeof(Run));
	for (Run& run : runs_stdlib) {
		test_sort_stdlib<K, IR>(
			items, run.num, run.seed, run.time
		);
	}
	TOGO_LOGF("std::stable_sort (%u):\n", num_runs);
	float const ratio_avg_stdlib = print_runs(runs_stdlib);

	TOGO_LOG("\ncomparison (scale = stdlib time / radix time):\n");
	float scale_accum = 0.0f;
	for (unsigned i = 0; i < num_runs; ++i) {
		float const scale
			= static_cast<float>(runs_stdlib[i].time)
			/ static_cast<float>(runs_radix[i].time)
		;
		scale_accum += scale;
		TOGO_LOGF(
			"num = %-10u  scale = %-6.03f\n",
			runs_radix[i].num,
			scale
		);
	}
	TOGO_LOGF(
		"ratio of ratio averages = %-6.03f  average scale = %-6.03f\n",
		ratio_avg_stdlib / ratio_avg_radix,
		scale_accum / num_runs
	);
}

signed
main() {
	memory_init();

	std::random_device rdev;

	test_and_validate<u8, u8, std::mt19937>(static_cast<u8>(-1), rdev());
	test_and_validate<u16, u8, std::mt19937>(static_cast<u8>(-1), rdev());
	test_and_validate<u32, u8, std::mt19937>(static_cast<u8>(-1), rdev());
	test_and_validate<u64, u8, std::mt19937_64>(static_cast<u8>(-1), rdev());

	TOGO_LOG("\n");
	test_and_validate<u8, u16, std::mt19937>(static_cast<u16>(-1), rdev());
	test_and_validate<u16, u16, std::mt19937>(static_cast<u16>(-1), rdev());
	test_and_validate<u32, u16, std::mt19937>(static_cast<u16>(-1), rdev());
	test_and_validate<u64, u16, std::mt19937_64>(static_cast<u16>(-1), rdev());

	TOGO_LOG("\n");
	test_and_validate<u8, u32, std::mt19937>(2e6, rdev());
	test_and_validate<u16, u32, std::mt19937>(2e6, rdev());
	test_and_validate<u32, u32, std::mt19937>(2e6, rdev());
	test_and_validate<u64, u32, std::mt19937_64>(2e6, rdev());

	TOGO_LOG("\n");
	test_and_validate<u8, u64, std::mt19937>(4e6, rdev());
	test_and_validate<u16, u64, std::mt19937>(4e6, rdev());
	test_and_validate<u32, u64, std::mt19937>(4e6, rdev());
	test_and_validate<u64, u64, std::mt19937_64>(4e6, rdev());

	TOGO_LOG("\noptimal (32-bit key):\n");
	test_growth<u32, u32, std::mt19937, std::random_device>(10, 1e5, rdev);

	TOGO_LOG("\nsub-optimal (64-bit key):\n");
	test_growth<u64, u64, std::mt19937, std::random_device>(10, 1e5, rdev);

	return 0;
}
