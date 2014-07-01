#line 2 "togo/file_io.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file file_io.hpp
@brief File IO.
@ingroup io
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/io_types.hpp>
#include <togo/io_proto.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/impl/file_io/posix.hpp>
#else
	#error "missing file_io implementation for target platform"
#endif

namespace togo {

/**
	@addtogroup io
	@{
*/

/**
	File reader.
*/
class FileReader
	: public IReader
{
public:
	FileStreamData _data;

	~FileReader() override;
	FileReader() = default;

	FileReader(FileReader const&) = delete;
	FileReader(FileReader&&) = delete;
	FileReader& operator=(FileReader const&) = delete;
	FileReader& operator=(FileReader&&) = delete;

	/// Attempt to open a file.
	///
	/// Returns false if the file could not be opened.
	bool open(char const* path);

	/// Close the stream.
	void close();

// IReader implementation
private:
	void read(void* data, u32 size) override;
};

/**
	File writer.
*/
class FileWriter
	: public IWriter
{
public:
	FileStreamData _data;

	~FileWriter() override;
	FileWriter() = default;

	FileWriter(FileWriter const&) = delete;
	FileWriter(FileWriter&&) = delete;
	FileWriter& operator=(FileWriter const&) = delete;
	FileWriter& operator=(FileWriter&&) = delete;

	/// Attempt to open a file.
	///
	/// Returns false if the file could not be opened.
	/// If append is true, the stream will be seeked to the end of
	/// the file if it already exists.
	bool open(char const* path, bool append);

	/// Close the stream.
	void close();

// IWriter implementation
private:
	void write(void const* data, u32 size) override;
};

/** @} */ // end of doc-group io

} // namespace togo
