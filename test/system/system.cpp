
#include <togo/log.hpp>
#include <togo/system.hpp>

using namespace togo;

signed
main() {
	TOGO_LOGF("num_cores = %u\n", system::num_cores());
	return 0;
}
