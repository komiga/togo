#line 2 "togo/core/filesystem/filesystem/linux.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/filesystem/filesystem.hpp>

#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <stdlib.h>

#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
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

inline static bool stat_wrapper(
	StringRef const& path,
	struct ::stat& stat_buf
) {
	signed const err = ::stat(path.data, &stat_buf);
	if (err != 0) {
		if (errno != ENOENT) {
			TOGO_LOG_DEBUGF(
				"stat_wrapper: errno = %d, %s\n",
				errno, std::strerror(errno)
			);
		}
		return false;
	}
	return true;
}

inline static bool fstat_wrapper(
	signed fd,
	struct ::stat& stat_buf
) {
	signed const err = ::fstat(fd, &stat_buf);
	if (err != 0) {
		if (errno != ENOENT) {
			TOGO_LOG_DEBUGF(
				"fstat_wrapper: errno = %d, %s\n",
				errno, std::strerror(errno)
			);
		}
		return false;
	}
	return true;
}

bool filesystem::is_file(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!stat_wrapper(path, stat_buf)) {
		return false;
	}
	return S_ISREG(stat_buf.st_mode);
}

bool filesystem::is_directory(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!stat_wrapper(path, stat_buf)) {
		return false;
	}
	return S_ISDIR(stat_buf.st_mode);
}

u64 filesystem::time_last_modified(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!stat_wrapper(path, stat_buf)) {
		return 0;
	}
	return static_cast<u64>(stat_buf.st_ctime);
}

u64 filesystem::file_size(StringRef const& path) {
	struct ::stat stat_buf{};
	if (!stat_wrapper(path, stat_buf) || !S_ISREG(stat_buf.st_mode)) {
		return 0;
	}
	TOGO_ASSERTE(stat_buf.st_size >= 0);
	return static_cast<u64>(stat_buf.st_size);
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

bool filesystem::move_file(StringRef const& src, StringRef const& dest) {
	signed err = ::link(src.data, dest.data);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"move_file: link(): errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	err = ::unlink(src.data);
	if (err != 0) {
		TOGO_LOG_DEBUGF(
			"move_file: unlink(): errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		return false;
	}
	return true;
}

bool filesystem::copy_file(StringRef const& src, StringRef const& dest, bool overwrite) {
	bool success = false;
	signed fd_src, fd_dest;
	struct ::stat stat_buf{};
	mode_t mode = 0;
	off_t size = 0;
	off_t offset = 0;

	fd_src = ::open(src.data, O_RDONLY);
	if (fd_src == -1) {
		TOGO_LOG_DEBUGF(
			"copy_file: open(src): errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		goto l_exit;
	}
	if (!fstat_wrapper(fd_src, stat_buf)) {
		goto l_close_src;
	}

	mode = stat_buf.st_mode;
	size = stat_buf.st_size;

	fd_dest = ::open(
		dest.data,
		O_CREAT | O_WRONLY | (overwrite ? O_TRUNC : O_EXCL),
		mode
	);
	if (fd_dest == -1) {
		TOGO_LOG_DEBUGF(
			"copy_file: create(dest): errno = %d, %s\n",
			errno, std::strerror(errno)
		);
		goto l_close_src;
	}
	if (!fstat_wrapper(fd_dest, stat_buf)) {
		goto l_close;
	}
	if (mode != stat_buf.st_mode) {
		if (::fchmod(fd_dest, mode) == -1) {
			TOGO_LOG_DEBUGF(
				"copy_file: fchmod(dest, mode): errno = %d, %s\n",
				errno, std::strerror(errno)
			);
			goto l_close;
		}
	}

	while (offset != size) {
		auto written = ::sendfile(fd_dest, fd_src, &offset, size - offset);
		if (written == -1) {
			if (errno == EAGAIN) {
				continue;
			}
			TOGO_LOG_DEBUGF(
				"copy_file: sendfile(dest, src): errno = %d, %s\n",
				errno, std::strerror(errno)
			);
			goto l_close;
		}
	}

	success = true;

l_close:
	if (::close(fd_dest) != 0) {
		TOGO_LOG_DEBUGF(
			"copy_file: close(dest): errno = %d, %s\n",
			errno, std::strerror(errno)
		);
	}

l_close_src:
	if (::close(fd_src) != 0) {
		TOGO_LOG_DEBUGF(
			"copy_file: close(src): errno = %d, %s\n",
			errno, std::strerror(errno)
		);
	}

l_exit:
	return success;
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
