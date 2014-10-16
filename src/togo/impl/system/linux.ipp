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
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <fcntl.h>

namespace togo {

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

bool system::set_working_dir(StringRef const& path) {
	signed const err = ::chdir(path.data);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"set_working_dir: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	return true;
}

inline static bool is_type_wrapper(
	StringRef const& path,
	struct ::stat& stat_buf
) {
	signed const err = ::stat(path.data, &stat_buf);
	if (err != 0) {
		if (errno != ENOENT) {
			TOGO_LOG_DEBUGF(
				"is_type_wrapper: errno = %d, %s\n",
				errno, std::strerror(errno)
			);
		}
		return false;
	}
	return true;
}

bool system::is_file(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!is_type_wrapper(path, stat_buf)) {
		return false;
	}
	return S_ISREG(stat_buf.st_mode);
}

bool system::is_directory(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!is_type_wrapper(path, stat_buf)) {
		return false;
	}
	return S_ISDIR(stat_buf.st_mode);
}

bool system::create_file(StringRef const& path) {
	signed const fd = ::open(
		path.data,
		O_CREAT | O_WRONLY | O_EXCL,
		// rw- rw- r--
		/* user  */ S_IRUSR | S_IWUSR |
		/* group */ S_IRGRP | S_IWGRP |
		/* other */ S_IROTH
	);
	if (fd == -1) {
		TOGO_LOG_DEBUGF(
			"create_file: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	if (::close(fd) != 0) {
		TOGO_LOG_DEBUGF(
			"create_file: close(): errno = %d, %s\n",
			errno, std::strerror(errno)
		);
	}
	return true;
}

bool system::remove_file(StringRef const& path) {
	signed const err = ::unlink(path.data);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"remove_file: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	return true;
}

bool system::create_directory(StringRef const& path) {
	signed const err = ::mkdir(
		path.data,
		// rwx rwx r-x
		/* user  */ S_IRWXU |
		/* group */ S_IRWXG |
		/* other */ S_IROTH | S_IXOTH
	);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"create_directory: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	return true;
}

bool system::remove_directory(StringRef const& path) {
	signed const err = ::rmdir(path.data);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"remove_directory: errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	return true;
}

} // namespace togo
