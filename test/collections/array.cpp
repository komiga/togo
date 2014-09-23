
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/memory.hpp>
#include <togo/array.hpp>

#include "../common/helpers.hpp"

using namespace togo;

#define ARRAY_ASSERTIONS(a, _size, _capacity) \
	TOGO_ASSERTE(array::size(a) == _size); \
	TOGO_ASSERTE(array::capacity(a) == _capacity); \
	TOGO_ASSERTE(array::empty(a) == (_size == 0)); \
	TOGO_ASSERTE(array::any(a) == (_size > 0))
//

signed
main() {
	memory_init();

	TOGO_LOGF("sizeof(Array<u32>) = %zu\n", sizeof(Array<u32>));
	TOGO_LOGF("alignof(Array<u32>) = %zu\n", alignof(Array<u32>));

	// Invariants
	Array<u32> a{memory::default_allocator()};
	ARRAY_ASSERTIONS(a, 0, 0);

	array::push_back(a, 42u);
	ARRAY_ASSERTIONS(a, 1, 8);

	array::clear(a);
	ARRAY_ASSERTIONS(a, 0, 8);

	// Insertion
	u32 count = 10;
	while (count--) {
		array::push_back(a, static_cast<u32>(10 - count));
	}
	ARRAY_ASSERTIONS(a, 10, 24);

	// Access
	count = 1;
	for (auto const v : a) {
		TOGO_ASSERTE(v == count);
		TOGO_LOGF("%d ", v);
		++count;
	}
	TOGO_LOG("\n");

	for (u32 i = 0; i < array::size(a); ++i) {
		TOGO_ASSERTE(a[i] == i + 1);
	}

	count = 1;
	for (auto it = array::begin(a); it != array::end(a); ++it) {
		TOGO_ASSERTE(*it == count);
		++count;
	}

	// Removal
	count = 5;
	while (count--) {
		array::pop_back(a);
	}
	ARRAY_ASSERTIONS(a, 5, 24);

	count = 5;
	while (count--) {
		array::pop_back(a);
	}
	ARRAY_ASSERTIONS(a, 0, 24);
	return 0;
}
