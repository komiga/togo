
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/fixed_array.hpp>

using namespace togo;

#define ARRAY_ASSERTIONS(a, _size, _empty) \
	TOGO_ASSERTE(fixed_array::size(a) == _size); \
	TOGO_ASSERTE(fixed_array::empty(a) == _empty); \
	TOGO_ASSERTE(fixed_array::any(a) != _empty)
//

signed
main() {
	TOGO_LOGF("sizeof(FixedArray<u32, 10>) = %zu\n", sizeof(FixedArray<u32, 10>));
	TOGO_LOGF("alignof(FixedArray<u32, 10>) = %zu\n", alignof(FixedArray<u32, 10>));

	// Invariants
	FixedArray<u32, 10> a{};
	TOGO_ASSERTE(fixed_array::capacity(a) == 10);
	ARRAY_ASSERTIONS(a, 0, true);

	fixed_array::push_back(a, 42u);
	ARRAY_ASSERTIONS(a, 1, false);

	fixed_array::clear(a);
	ARRAY_ASSERTIONS(a, 0, true);

	// Insertion
	u32 count = 10;
	while (count--) {
		fixed_array::push_back(a, static_cast<u32>(10 - count));
	}
	ARRAY_ASSERTIONS(a, 10, false);

	// Access
	count = 1;
	for (auto const v : a) {
		TOGO_ASSERTE(v == count);
		TOGO_LOGF("%d ", v);
		++count;
	}
	TOGO_LOG("\n");

	for (u32 i = 0; i < fixed_array::size(a); ++i) {
		TOGO_ASSERTE(a[i] == i + 1);
	}

	count = 1;
	for (auto it = fixed_array::begin(a); it != fixed_array::end(a); ++it) {
		TOGO_ASSERTE(*it == count);
		++count;
	}

	// Removal
	count = 5;
	while (count--) {
		fixed_array::pop_back(a);
	}
	ARRAY_ASSERTIONS(a, 5, false);

	count = 5;
	while (count--) {
		fixed_array::pop_back(a);
	}
	ARRAY_ASSERTIONS(a, 0, true);
	return 0;
}
