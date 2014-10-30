
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/hash_map.hpp>

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

signed main() {
	memory_init();

	TOGO_LOGF("sizeof(HashMap<hash32, u32>) = %zu\n", sizeof(HashMap<hash32, u32>));
	TOGO_LOGF("alignof(HashMap<hash32, u32>) = %zu\n", alignof(HashMap<hash32, u32>));

	{
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

		// Stress test
		hash_map::reserve(hm, 10000);
		HASH_MAP_ASSERTIONS(hm, 0, 10000);

		for (K key = 0; key < 10000; ++key) {
			hash_map::set(hm, key, {key});
			TOGO_ASSERTE(hash_map::has(hm, key));
			TOGO_ASSERTE(hash_map::count(hm, key) == 1);
		}
		HASH_MAP_ASSERTIONS(hm, 10000, 10000);
		TOGO_ASSERTE(hash_map::space(hm) == 0);

		for (auto const& entry : hm) {
			TOGO_ASSERTE(entry.key == entry.value.x);
		}
		for (K key = 0; key < 10000; ++key) {
			TOGO_ASSERTE(hash_map::has(hm, key));
			TOGO_ASSERTE(hash_map::count(hm, key) == 1);
		}

		for (K key = 10000; key > 0; --key) {
			hash_map::remove(hm, key - 1);
		}
		HASH_MAP_ASSERTIONS(hm, 0, 10000);
	}

	{
		using T = signed;
		using Node = HashMapNode<K, T>;
		K const key = K{1};
		T const* v;
		Node const* node;
		Node const* rm_node;

		HashMap<K, T> hm{memory::default_allocator()};
		TOGO_ASSERTE(hash_map::count(hm, key) == 0);

		// First value
		hash_map::push(hm, key, T{0});
		HASH_MAP_ASSERTIONS(hm, 1, 11);
		TOGO_ASSERTE(hash_map::count(hm, key) == 1);
		v = hash_map::get(hm, key);
		TOGO_ASSERTE(*v == T{0});

		// Overwrite first
		hash_map::set(hm, key, T{1});
		HASH_MAP_ASSERTIONS(hm, 1, 11);
		v = hash_map::get(hm, key);
		TOGO_ASSERTE(*v == T{1});

		// Second value
		hash_map::push(hm, key, T{2});
		HASH_MAP_ASSERTIONS(hm, 2, 11);
		TOGO_ASSERTE(hash_map::count(hm, key) == 2);
		v = hash_map::get(hm, key);
		TOGO_ASSERTE(*v == T{2});

		// Check node order
		node = hash_map::get_node(hm, key);
		TOGO_ASSERTE(node != nullptr);
		TOGO_ASSERTE(node->value == T{2});

		node = hash_map::get_next(hm, node);
		TOGO_ASSERTE(node != nullptr);
		TOGO_ASSERTE(node->value == T{1});
		rm_node = node;

		node = hash_map::get_next(hm, node);
		TOGO_ASSERTE(node == nullptr);

		TOGO_ASSERTE(hash_map::get_node(hm, K{2}) == nullptr);

		// Remove first value, T{1}
		hash_map::remove(hm, rm_node);
		HASH_MAP_ASSERTIONS(hm, 1, 11);
		TOGO_ASSERTE(hash_map::count(hm, key) == 1);
		rm_node = nullptr;

		// Check remaining nodes
		node = hash_map::get_node(hm, key);
		TOGO_ASSERTE(node != nullptr);
		TOGO_ASSERTE(node->value == T{2});
		rm_node = node;

		node = hash_map::get_next(hm, node);
		TOGO_ASSERTE(node == nullptr);

		// Remove second value, T{2}
		hash_map::remove(hm, rm_node);
		HASH_MAP_ASSERTIONS(hm, 0, 11);
		TOGO_ASSERTE(hash_map::count(hm, key) == 0);
		rm_node = nullptr;
	}
}
