
#include <togo/core/error/assert.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/fixed_allocator.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main() {
	FixedAllocator<1> a;
	TOGO_TEST_SHOULD_FAIL(a.allocate(10, 0));

	return 0;
}
