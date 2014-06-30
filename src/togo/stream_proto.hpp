#line 2 "togo/stream_proto.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file stream_proto.hpp
@brief Stream interface classes.
@ingroup stream
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/stream_types.hpp>

namespace togo {

/**
	@addtogroup stream
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
	virtual void read(void* data, u32 size) = 0;
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
	virtual void write(void const* data, u32 size) = 0;
};
inline IWriter::~IWriter() = default;

/** @} */ // end of doc-group stream

} // namespace togo
