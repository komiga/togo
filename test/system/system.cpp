
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/system.hpp>

using namespace togo;

signed
main() {
	TOGO_LOGF("num_cores = %u\n", system::num_cores());
	TOGO_LOG("Sleeping for 5 seconds\n");
	TOGO_LOGF("exec_dir: %s\n", system::exec_dir());
	TOGO_ASSERTE(system::secs_since_epoch() > 1407135980u);
	system::sleep_ms(2000u);
	return 0;
}
