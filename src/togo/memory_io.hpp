#line 2 "togo/memory_io.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file memory_io.hpp
@brief Memory IO.
@ingroup io
@ingroup memory_io
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/memory_types.hpp>
#include <togo/collection_types.hpp>
#include <togo/io_types.hpp>
#include <togo/io_proto.hpp>

namespace togo {

/**
	@addtogroup io
	@{
*/
/**
	@addtogroup memory_io
	@{
*/

/**
	Growing memory reader/writer.
*/
class MemoryStream
	: public virtual IStreamBase
	, public IStreamSeekable
	, public IReader
	, public IWriter
{
public:
	IOStatus _status;
	Array<u8> _buffer;
	u64 _position;

	~MemoryStream() override;

	/// Construct with allocator and initial capacity (in bytes).
	MemoryStream(Allocator& allocator, u32 const init_capacity);

	MemoryStream(MemoryStream const&) = delete;
	MemoryStream(MemoryStream&&) = delete;
	MemoryStream& operator=(MemoryStream const&) = delete;
	MemoryStream& operator=(MemoryStream&&) = delete;

	Array<u8>& buffer() noexcept {
		return _buffer;
	}

	/// Clear buffer and stream position.
	void clear();

private:
// IStreamBase implementation
	IOStatus status() const override;

// IStreamSeekable implementation
	u64 position() override;
	u64 seek_to(u64 position) override;
	u64 seek_relative(s64 offset) override;

// IReader implementation
	IOStatus read(void* data, unsigned size, unsigned* read_size) override;

// IWriter implementation
	IOStatus write(void const* data, unsigned size) override;
};

/** @} */ // end of doc-group memory_io
/** @} */ // end of doc-group io

} // namespace togo
