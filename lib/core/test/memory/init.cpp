
#include <togo/core/memory/memory.hpp>

using namespace togo;

signed main() {
	memory::init();
	memory::shutdown();
	return 0;
}
