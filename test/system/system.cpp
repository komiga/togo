
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/system.hpp>

using namespace togo;

signed
main() {
	TOGO_LOGF("num_cores = %u\n", system::num_cores());
	TOGO_LOG("Sleeping for 5 seconds\n");
	TOGO_ASSERTE(system::secs_since_epoch() > 1407135980);
	system::sleep_ms(5000);
	return 0;
}
