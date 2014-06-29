
#include <togo/config.hpp>
#include <togo/system.hpp>

#include <cstdio>

using namespace togo;

signed
main() {
	std::printf("num_cores = %u\n", system::num_cores());

	return 0;
}
