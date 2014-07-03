#line 2 "togo/io.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file io.hpp
@brief IO interface.
@ingroup io
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/io_types.hpp>
#include <togo/io_proto.hpp>
#include <togo/debug_constraints.hpp>

namespace togo {
namespace io {

/**
	@addtogroup io
	@{
*/

/// Get stream IO status.
inline IOStatus status(IReader const& stream) {
	return stream.status();
}

/// Get stream IO status.
inline IOStatus status(IWriter const& stream) {
	return stream.status();
}

/// Get stream position.
inline u64 position(IReader& stream) {
	return stream.position();
}

/// Get stream position.
inline u64 position(IWriter& stream) {
	return stream.position();
}

/// Seek stream to absolute position.
inline u64 seek_to(IReader& stream, u64 const pos) {
	return stream.seek_to(pos);
}

/// Seek stream to absolute position.
inline u64 seek_to(IWriter& stream, u64 const pos) {
	return stream.seek_to(pos);
}

/// Seek stream by an offset relative to the current position.
inline u64 seek_relative(IReader& stream, s64 const offset) {
	return stream.seek_relative(offset);
}

/// Seek stream by an offset relative to the current position.
inline u64 seek_relative(IWriter& stream, s64 const offset) {
	return stream.seek_relative(offset);
}

/// Read bytes into a buffer.
inline IOStatus read(IReader& stream, void* const buffer, unsigned const size) {
	return stream.read(buffer, size);
}

/// Write bytes from a buffer.
inline IOStatus write(IWriter& stream, void const* const buffer, unsigned const size) {
	return stream.write(buffer, size);
}

/// Read an arithmetic value.
template<class T>
inline IOStatus read_value(IReader& stream, T& value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::read(stream, &value, sizeof(T));
}

/// Read an arithmetic array.
template<class T>
inline IOStatus read_array(IReader& stream, T* const data, unsigned const count) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::read(stream, data, count * sizeof(T));
}

/// Write an arithmetic value.
template<class T>
inline IOStatus write_value(IWriter& stream, T const& value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::write(stream, &value, sizeof(T));
}

/// Write an arithmetic array.
template<class T>
inline IOStatus write_array(IWriter& stream, T const* const data, unsigned const count) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::write(stream, data, count * sizeof(T));
}

/** @} */ // end of doc-group io

} // namespace io
} // namespace togo
