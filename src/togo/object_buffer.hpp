#line 2 "togo/object_buffer.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file object_buffer.hpp
@brief Object buffer interface.
@ingroup object_buffer
*/

#pragma once

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/types.hpp>
#include <togo/memory_types.hpp>
#include <togo/array.hpp>
#include <togo/memory_io.hpp>
#include <togo/object_buffer_type.hpp>
#include <togo/io.hpp>

namespace togo {

/// Construct with allocator and initial capacity (in bytes).
template<class T, class S>
inline ObjectBuffer<T, S>::ObjectBuffer(
	Allocator& allocator,
	u32 const init_capacity
)
	: _stream(allocator, init_capacity)
	, _num_objects(0)
	, _consume_mode(false)
{}

namespace object_buffer {

/**
	@addtogroup object_buffer
	@{
*/

/// Number of objects.
template<class T, class S>
inline unsigned size(ObjectBuffer<T, S> const& ob) {
	return ob._num_objects;
}

/// Returns true if the buffer is empty.
template<class T, class S>
inline bool empty(ObjectBuffer<T, S> const& ob) {
	return ob._num_objects == 0;
}

/// Returns true if there is another object in the buffer.
///
/// This is always false if buffer is not in consume mode.
template<class T, class S>
inline bool has_more(ObjectBuffer<T, S> const& ob) {
	return ob._consume_mode && ob._num_objects > 0;
}

/// Reset stream and remove all objects.
template<class T, class S>
inline void clear(ObjectBuffer<T, S>& ob) {
	ob._stream.clear();
	ob._num_objects = 0;
}

/// Begin consume mode.
template<class T, class S>
inline void begin_consume(ObjectBuffer<T, S>& ob) {
	TOGO_ASSERT(!ob._consume_mode, "already in consume mode");
	ob._consume_mode = true;
	io::seek_to(ob._stream, 0);
}

/// End consume mode.
///
/// This will clear the buffer.
template<class T, class S>
inline void end_consume(ObjectBuffer<T, S>& ob) {
	TOGO_ASSERT(ob._consume_mode, "not in consume mode");
	clear(ob);
	ob._consume_mode = false;
}

/// Read object from a buffer.
///
/// This function fails if there are no more objects in the buffer.
/// Each read operation decrements the number of objects in the stream.
///
/// Returns the size of the object.
/// type is assigned to the deserialized type.
/// object either points to the object in the buffer or is null when
/// the object size is 0.
template<class T, class S, class E>
inline unsigned read(ObjectBuffer<T, S>& ob, E& type, void const*& object) {
	TOGO_DEBUG_ASSERT(ob._consume_mode, "not in consume mode");
	TOGO_ASSERT(ob._num_objects > 0, "no more objects remaining");
	T type_serial{0};
	S size_serial{0};
	TOGO_ASSERT(
		io::read_value(ob._stream, type_serial) &&
		io::read_value(ob._stream, size_serial),
		"failed to read from object buffer"
	);
	type = static_cast<E>(type_serial);
	if (size_serial > 0) {
		object = array::begin(ob._stream.get_buffer()) + ob._stream._position;
		io::seek_relative(ob._stream, size_serial);
	} else {
		object = nullptr;
	}
	--ob._num_objects;
	return static_cast<unsigned>(size_serial);
}

/// Write object to a buffer.
template<class T, class S, class E, class O>
inline void write(ObjectBuffer<T, S>& ob, E const type, O const& object) {
	TOGO_DEBUG_ASSERT(!ob._consume_mode, "cannot write in consume mode");
	TOGO_ASSERT(
		io::write_value(ob._stream, static_cast<T>(type)) &&
		io::write_value(ob._stream, static_cast<S>(sizeof_empty<O>())) &&
		(sizeof_empty<O>() == 0 || io::write(ob._stream, &object, sizeof(O))),
		"failed to write to object buffer"
	);
	++ob._num_objects;
}

/// Write empty object to buffer.
template<class T, class S, class E>
inline void write_empty(ObjectBuffer<T, S>& ob, E const type) {
	TOGO_DEBUG_ASSERT(!ob._consume_mode, "cannot write in consume mode");
	TOGO_ASSERT(
		io::write_value(ob._stream, static_cast<T>(type)) &&
		io::write_value(ob._stream, static_cast<S>(0)),
		"failed to write to object buffer"
	);
	++ob._num_objects;
}

/** @} */ // end of doc-group object_buffer

} // namespace object_buffer

} // namespace togo
