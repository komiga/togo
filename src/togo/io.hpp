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

/// Read bytes into a buffer.
inline void read(IReader& stream, void* const buffer, u32 const size) {
	stream.read(buffer, size);
}

/// Write bytes from a buffer.
inline void write(IWriter& stream, void const* const buffer, u32 const size) {
	stream.write(buffer, size);
}

/// Read an arithmetic value (by reference).
template<class T>
inline void read_value(IReader& stream, T& value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	io::read(stream, &value, sizeof(T));
}

/// Read an arithmetic value (by value).
template<class T>
inline T read_value(IReader& stream) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	T value{};
	io::read(stream, &value, sizeof(T));
	return value;
}

/// Read an arithmetic array.
template<class T>
inline void read_array(IReader& stream, T* const data, u32 const count) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	io::read(stream, data, count * sizeof(T));
}

/// Write an arithmetic value.
template<class T>
inline void write_value(IWriter& stream, T const& value) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	io::write(stream, &value, sizeof(T));
}

/// Write an arithmetic array.
template<class T>
inline void write_array(IWriter& stream, T const* const data, u32 const count) {
	TOGO_CONSTRAIN_ARITHMETIC(T);
	io::write(stream, data, count * sizeof(T));
}

/** @} */ // end of doc-group io

} // namespace io
} // namespace togo
