#line 2 "togo/impl/filesystem/linux.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/utility.hpp>
#include <togo/string.hpp>
#include <togo/log.hpp>
#include <togo/filesystem.hpp>

#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <stdlib.h>

#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace togo {

StringRef filesystem::exec_dir() {
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

unsigned filesystem::working_dir(char* str, unsigned capacity) {
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

bool filesystem::set_working_dir(StringRef const& path) {
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

bool filesystem::is_file(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!is_type_wrapper(path, stat_buf)) {
		return false;
	}
	return S_ISREG(stat_buf.st_mode);
}

bool filesystem::is_directory(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!is_type_wrapper(path, stat_buf)) {
		return false;
	}
	return S_ISDIR(stat_buf.st_mode);
}

bool filesystem::create_file(StringRef const& path) {
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

bool filesystem::remove_file(StringRef const& path) {
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

bool filesystem::create_directory(StringRef const& path) {
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

bool filesystem::remove_directory(StringRef const& path) {
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
