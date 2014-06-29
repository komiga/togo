#line 2 "togo/queue.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file queue.hpp
@brief Queue interface.
@ingroup queue
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/collection_types.hpp>
#include <togo/assert.hpp>
#include <togo/array.hpp>

#include <cstring>

namespace togo {

/// Construct with allocator for storage.
///
/// Queue is empty and has null storage until it grows (either
/// explicitly through reserve() or by insertion).
template<class T>
inline Queue<T>::Queue(Allocator& allocator)
	: _data(allocator)
	, _head(0)
	, _size(0)
{}

/// Access value by index.
template<class T>
inline T& Queue<T>::operator[](u32 const i) {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[(_head + i) % array::size(_data)];
}

/// Access value by index.
template<class T>
inline T const& Queue<T>::operator[](u32 const i) const {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[(_head + i) % array::size(_data)];
}

namespace queue {

namespace {

template<class T>
void set_capacity(Queue<T>& q, u32 const new_capacity) {
	if (new_capacity <= q._size) {
		return;
	}
	u32 const old_capacity = array::size(q._data);
	array::resize(q._data, new_capacity);
	if (q._head + q._size > old_capacity) {
		// Push items from head to the end of the array
		u32 const to_end_count = old_capacity - q._head;
		std::memmove(
			array::begin(q._data) + new_capacity - to_end_count,
			array::begin(q._data) + q._head,
			to_end_count * sizeof(T)
		);
		q._head += new_capacity - old_capacity;
	}
}

template<class T>
void grow(Queue<T>& q, u32 const min_capacity = 0) {
	u32 new_capacity = array::size(q._data) * 2 + 8;
	if (new_capacity < min_capacity) {
		new_capacity = min_capacity;
	}
	queue::set_capacity(q, new_capacity);
}

} // anonymous namespace

/**
	@addtogroup queue
	@{
*/

/// Number of items.
template<class T>
inline u32 size(Queue<T> const& q) { return q._size; }

/// Number of items reserved.
template<class T>
inline u32 capacity(Queue<T> const& q) { return array::size(q._data); }

/// Number of items that can be pushed before the queue grows.
template<class T>
inline u32 space(Queue<T> const& q) { return array::size(q._data) - q._size; }

/// Returns true if there are any items in the queue.
template<class T>
inline bool any(Queue<T> const& q) { return q._size != 0; }

/// Returns true if there are no items in the queue.
template<class T>
inline bool empty(Queue<T> const& q) { return q._size == 0; }

/// Access first item.
template<class T>
inline T& front(Queue<T>& q) {
	return q[0];
}
/// Access first item.
template<class T>
inline T const& front(Queue<T> const& q) {
	return q[0];
}

/// Access last item.
template<class T>
inline T& back(Queue<T>& q) {
	return q[q._size - 1];
}
/// Access last item.
template<class T>
inline T const& back(Queue<T> const& q) {
	return q[q._size - 1];
}

/// Reserve at least new_capacity.
///
/// If new_capacity is less than the current size, this function
/// has no effect. Assume any references are invalid after call.
template<class T>
inline void reserve(Queue<T>& q, u32 const new_capacity) {
	queue::set_capacity(q, new_capacity);
}

/// Remove all items.
template<class T>
inline void clear(Queue<T>& q) {
	q._head = 0;
	q._size = 0;
}

/// Push an item to the back of the queue.
template<class T>
inline void push_back(Queue<T>& q, T const& item) {
	if (!queue::space(q)) {
		queue::grow(q);
	}
	q[q._size++] = item;
}

/// Remove an item from the back of the queue.
///
/// The queue must not be empty.
template<class T>
inline void pop_back(Queue<T>& q) {
	TOGO_DEBUG_ASSERTE(q._size != 0);
	if (!--q._size) {
		q._head = 0;
	}
}

/// Push an item to the front of the queue.
template<class T>
inline void push_front(Queue<T>& q, T const& item) {
	if (!queue::space(q)) {
		queue::grow(q);
	}
	q._head = (q._head - 1 + array::size(q._data)) % array::size(q._data);
	++q._size;
	q[0] = item;
}

/// Remove an item from the front of the queue.
///
/// The queue must not be empty.
template<class T>
inline void pop_front(Queue<T>& q) {
	TOGO_DEBUG_ASSERTE(q._size != 0);
	if (!--q._size) {
		q._head = 0;
	} else {
		q._head = (q._head + 1) % array::size(q._data);
	}
}

/** @} */ // end of doc-group queue

} // namespace queue

} // namespace togo
