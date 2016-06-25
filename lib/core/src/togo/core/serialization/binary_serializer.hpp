#line 2 "togo/core/serialization/binary_serializer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief BinarySerializer interface.
@ingroup lib_core_serialization
@ingroup lib_core_binary_serializer

@defgroup lib_core_binary_serializer BinarySerializer
@ingroup lib_core_serialization
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/serialization/types.hpp>
#include <togo/core/serialization/support.hpp>

namespace togo {

/**
	@addtogroup lib_core_binary_serializer
	@{
*/

/// Construct with stream.
inline BinaryInputSerializer::BinaryInputSerializer(IReader& stream, Endian endian)
	: _stream(stream)
	, _endian(endian)
{}

/// Construct with stream.
inline BinaryOutputSerializer::BinaryOutputSerializer(IWriter& stream, Endian endian)
	: _stream(stream)
	, _endian(endian)
{}

/** @cond INTERNAL */

// arithmetic

template<class T>
inline enable_if<is_arithmetic<T>::value>
read(serializer_tag, BinaryInputSerializer& ser, T& value) {
	TOGO_ASSERTE(io::read_value_endian(ser._stream, value, ser._endian));
}

template<class T>
inline enable_if<is_arithmetic<T>::value>
write(serializer_tag, BinaryOutputSerializer& ser, T const& value) {
	TOGO_ASSERTE(io::write_value_endian(ser._stream, value, ser._endian));
}

// arithmetic sequence

template<class T>
inline enable_if<is_arithmetic<T>::value>
read(serializer_tag, BinaryInputSerializer& ser, SerSequence<T>&& seq) {
	TOGO_ASSERTE(io::read_array_endian(ser._stream, array_ref(seq.ptr, seq.size), ser._endian));
}

template<class T>
inline enable_if<is_arithmetic<T>::value>
write(serializer_tag, BinaryOutputSerializer& ser, SerSequence<T> const& seq) {
	TOGO_ASSERTE(io::write_array_endian(ser._stream, array_cref(seq.ptr, seq.size), ser._endian));
}

// explicitly binary-serializable sequence

template<class T>
inline enable_if<is_binary_serializable_explicitly<T>::value>
read(serializer_tag, BinaryInputSerializer& ser, SerSequence<T>&& seq) {
	TOGO_ASSERTE(io::read(ser._stream, seq.ptr, seq.size * sizeof(T)));
}

template<class T>
inline enable_if<is_binary_serializable_explicitly<T>::value>
write(serializer_tag, BinaryOutputSerializer& ser, SerSequence<T> const& seq) {
	TOGO_ASSERTE(io::write(ser._stream, seq.ptr, seq.size * sizeof(T)));
}

// SerBuffer

inline void
read(serializer_tag, BinaryInputSerializer& ser, SerBuffer<false>&& buffer) {
	TOGO_ASSERTE(io::read(ser._stream, buffer.ptr, buffer.size));
}

template<bool C>
inline void
write(serializer_tag, BinaryOutputSerializer& ser, SerBuffer<C> const& buffer) {
	TOGO_ASSERTE(io::write(ser._stream, buffer.ptr, buffer.size));
}

/** @endcond */ // INTERNAL

/** @} */ // end of doc-group lib_core_binary_serializer

} // namespace togo
