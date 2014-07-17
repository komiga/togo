
#include <togo/log.hpp>
#include <togo/system.hpp>

using namespace togo;

signed
main() {
	TOGO_LOGF("num_cores = %u\n", system::num_cores());
	TOGO_LOG("Sleeping for 5 seconds\n");
	system::sleep_ms(5000);
	return 0;
}
