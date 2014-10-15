
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/fixed_array.hpp>
#include <togo/string.hpp>
#include <togo/system.hpp>

using namespace togo;

signed main() {
	TOGO_LOGF("num_cores = %u\n", system::num_cores());
	StringRef exec_dir = system::exec_dir();
	TOGO_LOGF("exec_dir: %.*s\n", exec_dir.size, exec_dir.data);
	TOGO_ASSERTE(system::secs_since_epoch() > 1407135980u);

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

	FixedArray<char, 256> wd_init{};
	unsigned const wd_init_size = system::working_dir(wd_init);
	TOGO_LOGF("wd_init: '%.*s'\n", string::size(wd_init), fixed_array::begin(wd_init));
	TOGO_ASSERTE(wd_init_size > 0 && wd_init_size == fixed_array::size(wd_init));

	TOGO_ASSERTE(system::set_working_dir("/"));
	FixedArray<char, 256> wd_root{};
	unsigned const wd_root_size = system::working_dir(wd_root);
	TOGO_LOGF("wd_root: '%.*s'\n", string::size(wd_root), fixed_array::begin(wd_root));
	TOGO_ASSERTE(wd_root_size > 0 && wd_root_size == fixed_array::size(wd_root));
	TOGO_ASSERTE(string::compare_equal(wd_root, "/"));

	TOGO_ASSERTE(system::set_working_dir(exec_dir));

	TOGO_LOG("\n");
	float const start = system::time_monotonic();
	system::sleep_ms(1000u);
	float const d_1000ms = system::time_monotonic() - start;
	system::sleep_ms(100u);
	float const d_100ms = system::time_monotonic() - start;
	system::sleep_ms(10u);
	float const d_10ms = system::time_monotonic() - start;
	system::sleep_ms(1u);
	float const d_1ms = system::time_monotonic() - start;

	TOGO_LOGF("start       : time_monotonic() = %.6f\n", start);
	TOGO_LOGF("1000ms delay: time_monotonic() = %.6f\n", d_1000ms);
	TOGO_LOGF("100ms delay : time_monotonic() = %.6f\n", d_100ms);
	TOGO_LOGF("10ms delay  : time_monotonic() = %.6f\n", d_10ms);
	TOGO_LOGF("1ms delay   : time_monotonic() = %.6f\n", d_1ms);

	return 0;
}
