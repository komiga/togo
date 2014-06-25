
#include <togo/memory.hpp>

#include <cstdio>

using namespace togo;

signed
main() {
	memory::init();
	memory::shutdown();
	return 0;
}
