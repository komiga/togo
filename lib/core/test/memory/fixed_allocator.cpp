
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/fixed_allocator.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main() {
	// NB: FixedAllocator doesn't need the memory system to be initialized
{
	FixedAllocator<1024> a;
	TOGO_ASSERTE(a._buffer == a._put);
}

{
	FixedAllocator<10> a;
	a.allocate(1, 0);
	a.allocate(4, 0);
	a.allocate(4, 0);
	a.allocate(1, 0);
	TOGO_ASSERTE((a._buffer + a.BUFFER_SIZE) == a._put);
}

{
	FixedAllocator<1> a;
	a.allocate(1, 0);
	TOGO_ASSERTE((a._buffer + a.BUFFER_SIZE) == a._put);

	TOGO_LOG("the following should fail:\n");
	a.allocate(1, 0);
	TOGO_ASSERT(false, "uhhh");
}

	return 0;
}
