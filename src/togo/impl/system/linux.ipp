#line 2 "togo/impl/system/linux.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/utility.hpp>
#include <togo/string.hpp>
#include <togo/log.hpp>
#include <togo/system.hpp>

#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <time.h>

#include <linux/limits.h>

namespace togo {

unsigned system::num_cores() {
	signed const num = sysconf(_SC_NPROCESSORS_ONLN);
	return static_cast<unsigned>(max(1, num));
}

void system::sleep_ms(unsigned duration_ms) {
	timespec duration;
	timespec remaining;
	duration.tv_sec = duration_ms / 1000;
	duration.tv_nsec = (duration_ms % 1000) * 1000000; // 10^6
	signed err = 0;
	do {
		err = nanosleep(&duration, &remaining);
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
	signed const err = clock_gettime(CLOCK_MONOTONIC, &ts);
	TOGO_ASSERTF(
		err == 0,
		"clock_gettime() with CLOCK_MONOTONIC failed; errno = %d, %s\n",
		errno,
		std::strerror(errno)
	);
	return static_cast<float>(ts.tv_sec) + static_cast<float>(ts.tv_nsec) / 1e9;
}

StringRef system::exec_dir() {
	static unsigned exec_dir_str_size{0};
	static char exec_dir_str[PATH_MAX]{'\0'};
	if (exec_dir_str_size == 0) {
		ssize_t size = ::readlink(
			"/proc/self/exe",
			exec_dir_str,
			array_extent(exec_dir_str) - 1
		);
		if (size == -1) {
			TOGO_LOG_DEBUGF(
				"exec_dir: errno = %d, %s\n",
				errno, std::strerror(errno)
			);
		} else {
			for (ssize_t i = size - 1; i >= 0; --i) {
				if (exec_dir_str[i] == '/') {
					size = i;
					break;
				}
			}
			exec_dir_str_size = max(1u, static_cast<unsigned>(size));
			exec_dir_str[exec_dir_str_size] = '\0';
		}
	}
	return {exec_dir_str, exec_dir_str_size};
}

unsigned system::working_dir(char* str, unsigned capacity) {
	TOGO_ASSERTE(str);
	if (::getcwd(str, capacity) == nullptr) {
		TOGO_LOG_DEBUGF(
			"working_dir: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return 0;
	}
	return string::size(str);
}

bool system::change_working_dir(StringRef const& path) {
	signed const err = ::chdir(path.data);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"change_working_dir: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	return true;
}

} // namespace togo
