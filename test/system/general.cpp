
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

	FixedArray<char, 256> wd_init{};
	unsigned const wd_init_size = system::working_dir(wd_init);
	TOGO_LOGF("wd_init: '%.*s'\n", string::size(wd_init), fixed_array::begin(wd_init));
	TOGO_ASSERTE(wd_init_size > 0 && wd_init_size == fixed_array::size(wd_init));

	system::set_working_dir("/");
	FixedArray<char, 256> wd_root{};
	unsigned const wd_root_size = system::working_dir(wd_root);
	TOGO_LOGF("wd_root: '%.*s'\n", string::size(wd_root), fixed_array::begin(wd_root));
	TOGO_ASSERTE(wd_root_size > 0 && wd_root_size == fixed_array::size(wd_root));
	TOGO_ASSERTE(string::compare_equal(wd_root, "/"));

	TOGO_LOG("\n");
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
