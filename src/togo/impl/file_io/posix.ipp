#line 2 "togo/impl/file_io/posix.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/file_io.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>

namespace togo {

inline bool
file_open(
	PosixFileStreamData& data,
	char const* const path,
	char const* const mode
) {
	TOGO_ASSERT(!data.handle, "cannot open new path on an open stream");
	data.handle = std::fopen(path, mode);
	if (!data.handle) {
		TOGO_LOG_DEBUGF(
			"failed to open file '%s' for reading: %d, %s",
			path, errno, std::strerror(errno)
		);
		return false;
	}
	return true;
}

inline void
file_close(
	PosixFileStreamData& data
) {
	if (data.handle) {
		if (std::fclose(data.handle)) {
			TOGO_LOG_DEBUGF(
				"failed to close file stream: %d, %s",
				errno, std::strerror(errno)
			);
		}
		data.handle = nullptr;
	}
}

// FileReader implementation

FileReader::~FileReader() {
	this->close();
}

bool FileReader::open(char const* const path) {
	return file_open(_data, path, "r");
}

void FileReader::close() {
	file_close(_data);
}

void FileReader::read(void* const data, u32 const size) {
	TOGO_DEBUG_ASSERT(_data.handle, "cannot perform IO on a closed stream");
	std::size_t const read_size = std::fread(data, 1, size, _data.handle);
	if (read_size != size) {
		if (std::feof(_data.handle)) {
			TOGO_ASSERTF(
				false, "failed to read requested size (%zu != %u): EOF",
				read_size, size
			);
		} else {
			TOGO_ASSERTF(
				false, "failed to read requested size (%zu != %u): %d, %s",
				read_size, size, errno, std::strerror(errno)
			);
		}
	}
}

// FileWriter implementation

FileWriter::~FileWriter() {
	this->close();
}

bool FileWriter::open(char const* const path, bool const append) {
	return file_open(_data, path, append ? "a" : "w");
}

void FileWriter::close() {
	file_close(_data);
}

void FileWriter::write(void const* const data, u32 const size) {
	TOGO_DEBUG_ASSERT(_data.handle, "cannot perform IO on a closed stream");
	std::size_t const write_size = std::fwrite(data, 1, size, _data.handle);
	if (write_size != size) {
		TOGO_ASSERTF(
			false, "failed to write requested size (%zu != %u): %d, %s",
			write_size, size, errno, std::strerror(errno)
		);
	}
}

} // namespace togo
