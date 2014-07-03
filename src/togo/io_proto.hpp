#line 2 "togo/io_proto.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file io_proto.hpp
@brief IO interface classes.
@ingroup io
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/io_types.hpp>
#include <togo/utility.hpp>

namespace togo {

/**
	@addtogroup io
	@{
*/

/**
	Stream reader interface.
*/
class IReader {
public:
	IReader() = default;
	IReader(IReader const&) = default;
	IReader(IReader&&) = default;
	IReader& operator=(IReader const&) = default;
	IReader& operator=(IReader&&) = default;

	virtual ~IReader() = 0;

	virtual IOStatus status() const = 0;
	virtual u64 position() = 0;
	virtual u64 seek_to(u64 position) = 0;
	virtual u64 seek_relative(s64 offset) = 0;
	virtual IOStatus read(void* data, unsigned size) = 0;
};
inline IReader::~IReader() = default;

/**
	Stream writer interface.
*/
class IWriter {
public:
	IWriter() = default;
	IWriter(IWriter const&) = default;
	IWriter(IWriter&&) = default;
	IWriter& operator=(IWriter const&) = default;
	IWriter& operator=(IWriter&&) = default;

	virtual ~IWriter() = 0;

	virtual IOStatus status() const = 0;
	virtual u64 position() = 0;
	virtual u64 seek_to(u64 position) = 0;
	virtual u64 seek_relative(s64 offset) = 0;
	virtual IOStatus write(void const* data, unsigned size) = 0;
};
inline IWriter::~IWriter() = default;

/** @} */ // end of doc-group io

} // namespace togo
