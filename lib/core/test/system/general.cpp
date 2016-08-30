
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/system/system.hpp>

using namespace togo;

signed main() {
	TOGO_LOGF("pid = %u, num_cores = %u\n", system::pid(), system::num_cores());

	StringRef const hostname = system::hostname();
	TOGO_LOGF("hostname = %.*s\n", hostname.size, hostname.data);

	TOGO_ASSERTE(system::secs_since_epoch() > 1407135980u);

	// Environment variables
	StringRef const env_path = system::environment_variable("PATH");
	TOGO_LOGF("PATH = '%.*s'\n", env_path.size, env_path.data);

	StringRef const env_test_name = "__TOGO_TEST";

	// Base state of unassigned variable
	StringRef env_test_value = system::environment_variable(env_test_name);
	TOGO_ASSERTE(!env_test_value.valid());

	// Assignment
	TOGO_ASSERTE(system::set_environment_variable(env_test_name, "test"));
	env_test_value = system::environment_variable(env_test_name);
	TOGO_ASSERTE(string::compare_equal(env_test_value, "test"));

	// Removal (return to unassigned state)
	TOGO_ASSERTE(system::remove_environment_variable(env_test_name));
	env_test_value = system::environment_variable(env_test_name);
	TOGO_ASSERTE(!env_test_value.valid());

	// Timers
	TOGO_LOG("\n");
	f64 const start = system::time_monotonic();
	system::sleep_ms(1000u);
	f64 const d_1000ms = system::time_monotonic() - start;
	system::sleep_ms(100u);
	f64 const d_100ms = system::time_monotonic() - start;
	system::sleep_ms(10u);
	f64 const d_10ms = system::time_monotonic() - start;
	system::sleep_ms(1u);
	f64 const d_1ms = system::time_monotonic() - start;

	TOGO_LOGF("start       : time_monotonic() = %.6f\n", start);
	TOGO_LOGF("1000ms delay: time_monotonic() = %.6f\n", d_1000ms);
	TOGO_LOGF("100ms delay : time_monotonic() = %.6f\n", d_100ms);
	TOGO_LOGF("10ms delay  : time_monotonic() = %.6f\n", d_10ms);
	TOGO_LOGF("1ms delay   : time_monotonic() = %.6f\n", d_1ms);

	return 0;
}
