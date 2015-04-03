#line 2 "togo/core/io/memory_stream.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Memory IO.
@ingroup lib_core_io
@ingroup lib_core_io_memory

@defgroup lib_core_io_memory Memory IO
@ingroup lib_core_io
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/io/proto.hpp>

namespace togo {

/**
	@addtogroup lib_core_io_memory
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
	Array<u8> _data;
	u64 _position;

	~MemoryStream() override;

	/// Construct with allocator and initial capacity (in bytes).
	MemoryStream(Allocator& allocator, u32 const init_capacity);

	MemoryStream(MemoryStream const&) = delete;
	MemoryStream(MemoryStream&&) = delete;
	MemoryStream& operator=(MemoryStream const&) = delete;
	MemoryStream& operator=(MemoryStream&&) = delete;

	/// Size.
	u32_fast size() noexcept {
		return _data._size;
	}

	/// Data.
	Array<u8>& data() noexcept {
		return _data;
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
	u8 const* _data;
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

	/// Size.
	u32_fast size() noexcept {
		return _size;
	}

	/// Data.
	u8 const* data() noexcept {
		return _data;
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

/** @} */ // end of doc-group lib_core_io_memory

} // namespace togo
