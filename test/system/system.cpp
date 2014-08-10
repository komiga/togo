
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
	float const start = system::time_monotonic();
	TOGO_LOGF("start       : time_monotonic() = %.6f\n", start);
	system::sleep_ms(1000u);
	TOGO_LOGF("1000ms delay: time_monotonic() = %.6f\n", system::time_monotonic() - start);
	system::sleep_ms(100u);
	TOGO_LOGF("100ms delay : time_monotonic() = %.6f\n", system::time_monotonic() - start);
	system::sleep_ms(10u);
	TOGO_LOGF("10ms delay  : time_monotonic() = %.6f\n", system::time_monotonic() - start);
	system::sleep_ms(1u);
	TOGO_LOGF("1ms delay   : time_monotonic() = %.6f\n", system::time_monotonic() - start);
	return 0;
}
