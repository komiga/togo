#line 2 "togo/file_io.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief File IO.
@ingroup io
@ingroup file_io
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/string_types.hpp>
#include <togo/io_types.hpp>
#include <togo/io_proto.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/file_io/posix.hpp>
#else
	#error "missing file_io implementation for target platform"
#endif

namespace togo {

/**
	@addtogroup file_io
	@{
*/

/**
	File reader.
*/
class FileReader
	: public virtual IStreamBase
	, public IStreamSeekable
	, public IReader
{
public:
	FileStreamData _data{};

	~FileReader() override;
	FileReader() = default;

	FileReader(FileReader const&) = delete;
	FileReader(FileReader&&) = delete;
	FileReader& operator=(FileReader const&) = delete;
	FileReader& operator=(FileReader&&) = delete;

	/// Check if stream is open.
	bool is_open() const;

	/// Attempt to open a file.
	///
	/// Returns false if the file could not be opened.
	/// path must be NUL-terminated.
	bool open(StringRef const& path);

	/// Close the stream.
	void close();

private:
// IStreamBase implementation
	IOStatus status() const override;

// IStreamSeekable implementation
	u64 position() override;
	u64 seek_to(u64 position) override;
	u64 seek_relative(s64 offset) override;

// IReader implementation
	IOStatus read(void* data, unsigned size, unsigned* read_size) override;
};

/**
	File writer.
*/
class FileWriter
	: public virtual IStreamBase
	, public IStreamSeekable
	, public IWriter
{
public:
	FileStreamData _data{};

	~FileWriter() override;
	FileWriter() = default;

	FileWriter(FileWriter const&) = delete;
	FileWriter(FileWriter&&) = delete;
	FileWriter& operator=(FileWriter const&) = delete;
	FileWriter& operator=(FileWriter&&) = delete;

	/// Check if stream is open.
	bool is_open() const;

	/// Attempt to open a file.
	///
	/// Returns false if the file could not be opened.
	/// If append is true, the stream will be seeked to the end of
	/// the file if it already exists.
	/// path must be NUL-terminated.
	bool open(StringRef const& path, bool append);

	/// Close the stream.
	void close();

private:
// IStreamBase implementation
	IOStatus status() const override;

// IStreamSeekable implementation
	u64 position() override;
	u64 seek_to(u64 position) override;
	u64 seek_relative(s64 offset) override;

// IWriter implementation
	IOStatus write(void const* data, unsigned size) override;
};

/** @} */ // end of doc-group file_io

} // namespace togo
