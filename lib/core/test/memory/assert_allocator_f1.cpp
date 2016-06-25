
#include <togo/core/error/assert.hpp>
#include <togo/core/memory/assert_allocator.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main() {
	AssertAllocator a;
	TOGO_TEST_SHOULD_FAIL(a.allocate(1, 0));

	return 0;
}
