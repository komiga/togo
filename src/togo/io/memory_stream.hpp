#line 2 "togo/io/memory_stream.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Memory IO.
@ingroup io
@ingroup io_memory
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/memory/types.hpp>
#include <togo/collection/types.hpp>
#include <togo/string/types.hpp>
#include <togo/io/types.hpp>
#include <togo/io/proto.hpp>

namespace togo {

/**
	@addtogroup io_memory
	@{
*/

/// Growing memory reader/writer.
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

/// Memory buffer reader.
class MemoryReader
	: public virtual IStreamBase
	, public IStreamSeekable
	, public IReader
{
public:
	IOStatus _status;
	u32_fast _size;
	u8 const* _buffer;
	u64 _position;

	~MemoryReader() override;

	/// Construct with buffer.
	MemoryReader(u8 const* const buffer, u32_fast const size);

	/// Construct with string reference.
	MemoryReader(StringRef const& ref);

	MemoryReader(MemoryReader const&) = delete;
	MemoryReader(MemoryReader&&) = delete;
	MemoryReader& operator=(MemoryReader const&) = delete;
	MemoryReader& operator=(MemoryReader&&) = delete;

	u32_fast size() noexcept {
		return _size;
	}

	u8 const* buffer() noexcept {
		return _buffer;
	}

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

/** @} */ // end of doc-group io_memory

} // namespace togo
