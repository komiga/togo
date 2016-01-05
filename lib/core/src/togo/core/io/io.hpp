#line 2 "togo/core/io/io.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief IO interface.
@ingroup lib_core_io
*/

#pragma once

// igen-source: io/io_li.cpp

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/utility/types.hpp>
#include <togo/core/utility/constraints.hpp>
#include <togo/core/utility/endian.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/io/proto.hpp>
#include <togo/core/lua/types.hpp>

#include <togo/core/io/io.gen_interface>

namespace togo {
namespace io {

/**
	@addtogroup lib_core_io
	@{
*/

/// Stream IO status.
inline IOStatus status(IStreamBase const& stream) {
	return stream.status();
}

/// Stream position.
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
inline IOStatus read_array(IReader& stream, ArrayRef<T> const& data) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::read(stream, begin(data), data.size() * sizeof(T));
}

/// Write an arithmetic array.
template<class T>
inline IOStatus write_array(IWriter& stream, ArrayRef<T const> const& data) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	return io::write(stream, begin(data), data.size() * sizeof(T));
}

/// Read an arithmetic value from endian.
///
/// If endian differs from the system, value is byte-reversed after reading.
template<class T>
inline IOStatus read_value_endian(IReader& stream, T& value, Endian const endian) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	auto status = io::read(stream, &value, sizeof(T));
	reverse_bytes_if(value, endian);
	return status;
}

/// Write an arithmetic value to endian.
///
/// If endian differs from the system, value is written byte-reversed.
template<class T>
inline IOStatus write_value_endian(IWriter& stream, T value, Endian const endian) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	reverse_bytes_if(value, endian);
	return io::write(stream, &value, sizeof(T));
}

/// Read an arithmetic array from endian.
///
/// If endian differs from the system, values are byte-reversed after reading.
template<class T>
inline IOStatus read_array_endian(
	IReader& stream,
	ArrayRef<T> const& data,
	Endian const endian
) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	auto status = io::read_array(stream, data);
	reverse_bytes_if(data, endian);
	return status;
}

/// Write an arithmetic array.
///
/// If endian differs from the system, values are written byte-reversed.
template<class T>
inline IOStatus write_array_endian(
	IWriter& stream,
	ArrayRef<T const> const& data,
	Endian const endian
) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	if (endian == Endian::system) {
		return io::write_array(stream, data);
	}
	T value;
	IOStatus status{IOStatus::flag_none};
	for (auto it = begin(data); it != end(data) && status; ++it) {
		value = reverse_bytes_copy(*it);
		status = io::write_value(stream, value);
	}
	return status;
}

/** @} */ // end of doc-group lib_core_io

} // namespace io
} // namespace togo
