#line 2 "togo/io/io.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief IO interface.
@ingroup io
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/io/types.hpp>
#include <togo/io/proto.hpp>
#include <togo/utility/constraints.hpp>

namespace togo {
namespace io {

/**
	@addtogroup io
	@{
*/

/// Get stream IO status.
inline IOStatus status(IStreamBase const& stream) {
	return stream.status();
}

/// Get stream position.
inline u64 position(IStreamSeekable& stream) {
	return stream.position();
}

/// Seek stream to absolute position.
inline u64 seek_to(IStreamSeekable& stream, u64 const pos) {
	return stream.seek_to(pos);
}

/// Seek stream by an offset relative to the current position.
inline u64 seek_relative(IStreamSeekable& stream, s64 const offset) {
	return stream.seek_relative(offset);
}

/// Read bytes into a buffer.
///
/// If read_size is non-null, its pointee will be assigned to the
/// number of bytes that were read from the stream, which may be less
/// than requested if the status is fail or EOF.
inline IOStatus read(
	IReader& stream,
	void* const buffer,
	unsigned const size,
	unsigned* const read_size = nullptr
) {
	return stream.read(buffer, size, read_size);
}

/// Write bytes from a buffer.
inline IOStatus write(
	IWriter& stream,
	void const* const buffer,
	unsigned const size
) {
	return stream.write(buffer, size);
}

/// Read an arithmetic value.
template<class T>
inline IOStatus read_value(IReader& stream, T& value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::read(stream, &value, sizeof(T));
}

/// Write an arithmetic value.
template<class T>
inline IOStatus write_value(IWriter& stream, T const& value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::write(stream, &value, sizeof(T));
}

/// Read an arithmetic array.
template<class T>
inline IOStatus read_array(
	IReader& stream,
	T* const data,
	unsigned const count
) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::read(stream, data, count * sizeof(T));
}

/// Write an arithmetic array.
template<class T>
inline IOStatus write_array(
	IWriter& stream,
	T const* const data,
	unsigned const count
) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::write(stream, data, count * sizeof(T));
}

/** @} */ // end of doc-group io

} // namespace io
} // namespace togo
