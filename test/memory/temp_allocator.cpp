
#include <togo/memory.hpp>
#include <togo/temp_allocator.hpp>

#include "../common/helpers.hpp"

#include <cstdio>

using namespace togo;

void f() {
	{
		TempAllocator<16> a;
	}

	std::puts("");
	{
		TempAllocator<32> a;
		void* p = a.allocate(24);
		p = a.allocate(1);
	}

	std::puts("");
	{
		TempAllocator<16> a;
		a.allocate(8);
		a.allocate(4087, 1);
		a.allocate(4087, 1);
		a.allocate(4087, 1);
		a.allocate(4087, 1);
		a.allocate(4087, 1);
	}
}

signed
main() {
	core_init();
	f();
	return 0;
}
