
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/array.hpp>

#include "../common/helpers.hpp"

using namespace togo;

#define ARRAY_ASSERTIONS(a, _size, _capacity) \
	TOGO_ASSERTE(array::size(a) == _size); \
	TOGO_ASSERTE(array::capacity(a) == _capacity); \
	TOGO_ASSERTE(array::empty(a) == (_size == 0)); \
	TOGO_ASSERTE(array::any(a) == (_size > 0))
//

signed main() {
	memory_init();

	TOGO_LOGF("sizeof(Array<s32>) = %zu\n", sizeof(Array<s32>));
	TOGO_LOGF("alignof(Array<s32>) = %zu\n", alignof(Array<s32>));

	// Invariants
	Array<s32> a{memory::default_allocator()};
	ARRAY_ASSERTIONS(a, 0, 0);

	array::push_back(a, 42);
	ARRAY_ASSERTIONS(a, 1, 8);

	array::clear(a);
	ARRAY_ASSERTIONS(a, 0, 8);

	// Insertion
	s32 count = 10;
	while (count--) {
		array::push_back(a, 10 - count);
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
		TOGO_ASSERTE(a[i] == signed_cast(i + 1));
	}

	count = 1;
	for (auto it = array::begin(a); it != array::end(a); ++it) {
		TOGO_ASSERTE(*it == count);
		++count;
	}

	// Copy
	Array<s32> copy{memory::default_allocator()};

	array::copy(copy, a);
	TOGO_ASSERTE(array::size(copy) == array::size(a));
	for (u32 i = 0; i < array::size(copy); ++i) {
		TOGO_ASSERTE(copy[i] == a[i]);
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

	/// Removal by index
	count = 4;
	while (count--) {
		array::push_back(a, 3 - count);
	}
	array::remove(a, 3);
	TOGO_ASSERTE(a[0] == 0);
	TOGO_ASSERTE(a[1] == 1);
	TOGO_ASSERTE(a[2] == 2);

	array::remove(a, 1);
	TOGO_ASSERTE(a[0] == 0);
	TOGO_ASSERTE(a[1] == 2);

	array::remove(a, 0);
	TOGO_ASSERTE(a[0] == 2);

	array::remove(a, 0);
	TOGO_ASSERTE(array::empty(a));

	// Remove-overwrite
	count = 4;
	while (count--) {
		array::push_back(a, 3 - count);
	}
	array::remove_over(a, 0);
	TOGO_ASSERTE(a[0] == 3);
	TOGO_ASSERTE(a[1] == 1);
	TOGO_ASSERTE(a[2] == 2);

	array::remove_over(a, 0);
	TOGO_ASSERTE(a[0] == 2);
	TOGO_ASSERTE(a[1] == 1);

	array::remove_over(a, 0);
	TOGO_ASSERTE(a[0] == 1);

	array::remove_over(a, 0);
	TOGO_ASSERTE(array::empty(a));

	return 0;
}
