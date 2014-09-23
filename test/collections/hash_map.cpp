
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/memory.hpp>
#include <togo/hash_map.hpp>

#include "../common/helpers.hpp"

using namespace togo;

#define HASH_MAP_ASSERTIONS(hm, _size, _capacity) \
	TOGO_ASSERTE(hash_map::size(hm) == _size); \
	TOGO_ASSERTE(hash_map::capacity(hm) == _capacity); \
	TOGO_ASSERTE(hash_map::empty(hm) == (_size == 0)); \
	TOGO_ASSERTE(hash_map::any(hm) == (_size > 0))
//

struct Value {
	u32 x;
};

using K = hash32;

void set_first(HashMap<K, Value>& hm) {
	Value* v;
	hash_map::set(hm, K{1}, {42});
	HASH_MAP_ASSERTIONS(hm, 1, 11);
	v = hash_map::get(hm, 1u);
	TOGO_ASSERTE(v != nullptr && v->x == 42);
	for (auto const& entry : hm) {
		TOGO_ASSERTE(entry.value.x == 42);
	}

	hash_map::set(hm, K{1}, {3});
	HASH_MAP_ASSERTIONS(hm, 1, 11);
	v = hash_map::get(hm, 1u);
	TOGO_ASSERTE(v != nullptr && v->x == 3);
	for (auto const& entry : hm) {
		TOGO_ASSERTE(entry.value.x == 3);
	}
}

signed
main() {
	memory_init();

	TOGO_LOGF("sizeof(HashMap<hash32, u32>) = %zu\n", sizeof(HashMap<hash32, u32>));
	TOGO_LOGF("alignof(HashMap<hash32, u32>) = %zu\n", alignof(HashMap<hash32, u32>));

	HashMap<K, Value> hm{memory::default_allocator()};
	HASH_MAP_ASSERTIONS(hm, 0, 0);
	set_first(hm);

	hash_map::clear(hm);
	HASH_MAP_ASSERTIONS(hm, 0, 11);
	set_first(hm);

	hash_map::remove(hm, K{16});
	HASH_MAP_ASSERTIONS(hm, 1, 11);

	hash_map::remove(hm, K{1});
	HASH_MAP_ASSERTIONS(hm, 0, 11);

	hash_map::reserve(hm, 0);
	HASH_MAP_ASSERTIONS(hm, 0, 11);

	hash_map::reserve(hm, 20);
	HASH_MAP_ASSERTIONS(hm, 0, 20);

	hash_map::reserve(hm, 1000);
	HASH_MAP_ASSERTIONS(hm, 0, 1000);

	for (K key = 0; key < 1000; ++key) {
		hash_map::set(hm, key, {key});
		TOGO_ASSERTE(hash_map::has(hm, key));
	}
	HASH_MAP_ASSERTIONS(hm, 1000, 1000);
	TOGO_ASSERTE(hash_map::space(hm) == 0);

	for (auto const& entry : hm) {
		TOGO_ASSERTE(entry.key == entry.value.x);
	}
	for (K key = 0; key < 1000; ++key) {
		TOGO_ASSERTE(hash_map::has(hm, key));
	}

	for (K key = 999; key < 1000; --key) {
		hash_map::remove(hm, key);
	}
	HASH_MAP_ASSERTIONS(hm, 0, 1000);
}
