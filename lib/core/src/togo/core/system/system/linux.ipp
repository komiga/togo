#line 2 "togo/core/system/system/linux.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/system/system.hpp>

#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>

namespace togo {

unsigned system::pid() {
	pid_t id = getpid();
	TOGO_DEBUG_ASSERT(id >= 0, "unexpected pid < 0, what does it mean?");
	return static_cast<unsigned>(id);
}

unsigned system::num_cores() {
	signed const num = ::sysconf(_SC_NPROCESSORS_ONLN);
	return static_cast<unsigned>(max(1, num));
}

void system::sleep_ms(unsigned duration_ms) {
	timespec duration;
	timespec remaining;
	duration.tv_sec = duration_ms / 1000;
	duration.tv_nsec = (duration_ms % 1000) * 1000000; // 10^6
	signed err = 0;
	do {
		err = ::nanosleep(&duration, &remaining);
	} while (err != 0 && errno == EINTR);
	if (err != 0 && errno != 0) {
		TOGO_LOG_DEBUGF(
			"sleep_ms: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
	}
}

float system::time_monotonic() {
	timespec ts;
	signed const err = ::clock_gettime(CLOCK_MONOTONIC, &ts);
	TOGO_ASSERTF(
		err == 0,
		"clock_gettime() with CLOCK_MONOTONIC failed; errno = %d, %s\n",
		errno,
		std::strerror(errno)
	);
	return static_cast<float>(ts.tv_sec) + static_cast<float>(ts.tv_nsec) / 1e9;
}

StringRef system::environment_variable(StringRef const& name) {
	char const* const value = ::getenv(name.data);
	return {value, cstr_tag{}};
}

bool system::set_environment_variable(
	StringRef const& name,
	StringRef const& value
) {
	signed const err = ::setenv(name.data, value.data, 1/*true*/);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"set_environment_variable: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
	}
	return err == 0;
}

bool system::remove_environment_variable(StringRef const& name) {
	// NB: unsetenv() succeeds even if the variable does not exist
	signed const err = ::unsetenv(name.data);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"remove_environment_variable: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
	}
	return err == 0;
}

} // namespace togo
