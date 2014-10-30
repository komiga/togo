#line 2 "togo/io/file_stream/posix.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/string/string.hpp>
#include <togo/log/log.hpp>
#include <togo/io/file_stream.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>

namespace togo {

inline bool
file_is_open(
	PosixFileStreamData const& data
) {
	return data.handle != nullptr;
}

inline bool
file_open(
	PosixFileStreamData& data,
	StringRef const& path,
	char const* const mode
) {
	TOGO_ASSERT(!data.handle, "cannot open new path on an open stream");
	data.handle = std::fopen(path.data, mode);
	if (!data.handle) {
		TOGO_LOG_DEBUGF(
			"failed to open file '%.*s' for reading: %d, %s\n",
			path.size, path.data, errno, std::strerror(errno)
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
				"failed to close file stream: %d, %s\n",
				errno, std::strerror(errno)
			);
		}
		data.handle = nullptr;
	}
}

inline void
file_set_status(
	PosixFileStreamData& data
) {
	TOGO_DEBUG_ASSERT(data.handle, "cannot perform operation on a closed stream");
	bool const err = !!std::ferror(data.handle);
	bool const eof = !!std::feof(data.handle);
	data.status.assign(err, eof);
}

inline u64
file_position(
	PosixFileStreamData& data
) {
	TOGO_DEBUG_ASSERT(data.handle, "cannot perform operation on a closed stream");
	off_t const lpos = ftello(data.handle);
	if (lpos < 0) {
		TOGO_LOG_DEBUGF(
			"failed to obtain stream position: %d, %s\n",
			errno, std::strerror(errno)
		);
		data.status._value |= IOStatus::flag_fail;
		return 0;
	} else {
		return static_cast<u64>(lpos);
	}
}

inline u64
file_seek_to(
	PosixFileStreamData& data,
	u64 const position
) {
	TOGO_DEBUG_ASSERT(data.handle, "cannot perform operation on a closed stream");
	if (fseeko(data.handle, static_cast<off_t>(position), SEEK_SET)) {
		TOGO_LOG_DEBUGF(
			"failed to seek to %lu: %d, %s\n",
			position, errno, std::strerror(errno)
		);
	}
	file_set_status(data);
	return file_position(data);
}

inline u64
file_seek_relative(
	PosixFileStreamData& data,
	s64 const offset
) {
	TOGO_DEBUG_ASSERT(data.handle, "cannot perform operation on a closed stream");
	if (fseeko(data.handle, static_cast<off_t>(offset), SEEK_CUR)) {
		TOGO_LOG_DEBUGF(
			"failed to seek by offset %ld: %d, %s\n",
			offset, errno, std::strerror(errno)
		);
	}
	file_set_status(data);
	return file_position(data);
}

// FileReader implementation

FileReader::~FileReader() {
	this->close();
}

bool FileReader::is_open() const {
	return file_is_open(_data);
}

bool FileReader::open(StringRef const& path) {
	return file_open(_data, path, "rb");
}

void FileReader::close() {
	file_close(_data);
}

IOStatus FileReader::status() const {
	return _data.status;
}

u64 FileReader::position() {
	return file_position(_data);
}

u64 FileReader::seek_to(u64 const position) {
	return file_seek_to(_data, position);
}

u64 FileReader::seek_relative(s64 const offset) {
	return file_seek_relative(_data, offset);
}

IOStatus FileReader::read(
	void* const data,
	unsigned const size,
	unsigned* const read_size
) {
	TOGO_DEBUG_ASSERT(_data.handle, "cannot perform IO on a closed stream");
	std::clearerr(_data.handle);
	std::size_t const op_read_size = std::fread(data, 1, size, _data.handle);
	if (op_read_size != size && errno != 0) {
		TOGO_LOG_DEBUGF(
			"failed to read requested size (%zu != %u): %d, %s\n",
			op_read_size, size, errno, std::strerror(errno)
		);
	}
	if (read_size) {
		*read_size = static_cast<unsigned>(op_read_size);
	}
	file_set_status(_data);
	return status();
}

// FileWriter implementation

FileWriter::~FileWriter() {
	this->close();
}

bool FileWriter::is_open() const {
	return file_is_open(_data);
}

bool FileWriter::open(StringRef const& path, bool const append) {
	return file_open(_data, path, append ? "ab" : "wb");
}

void FileWriter::close() {
	file_close(_data);
}

IOStatus FileWriter::status() const {
	return _data.status;
}

u64 FileWriter::position() {
	return file_position(_data);
}

u64 FileWriter::seek_to(u64 const position) {
	return file_seek_to(_data, position);
}

u64 FileWriter::seek_relative(s64 const offset) {
	return file_seek_relative(_data, offset);
}

IOStatus FileWriter::write(
	void const* const data,
	unsigned const size
) {
	TOGO_DEBUG_ASSERT(_data.handle, "cannot perform IO on a closed stream");
	std::clearerr(_data.handle);
	std::size_t const write_size = std::fwrite(data, 1, size, _data.handle);
	if (write_size != size) {
		TOGO_LOG_DEBUGF(
			"failed to write requested size (%zu != %u): %d, %s\n",
			write_size, size, errno, std::strerror(errno)
		);
	}
	file_set_status(_data);
	return status();
}

} // namespace togo
