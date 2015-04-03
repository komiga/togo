#line 2 "togo/core/collection/queue.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Queue interface.
@ingroup lib_core_collection
@ingroup lib_core_queue

@defgroup lib_core_queue Queue
@ingroup lib_core_collection
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/collection/array.hpp>

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

// Move constructor.
template<class T>
inline Queue<T>::Queue(Queue<T>&& other)
	: _data(rvalue_ref(other._data))
	, _head(other._head)
	, _size(other._size)
{
	other._head = 0;
	other._size = 0;
}

// Move assignment operator.
template<class T>
inline Queue<T>& Queue<T>::operator=(Queue<T>&& other) {
	_data = rvalue_ref(other._data);
	_head = other._head;
	_size = other._size;
	other._head = 0;
	other._size = 0;
	return *this;
}

/// Get value by index.
template<class T>
inline T& Queue<T>::operator[](unsigned const i) {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[(_head + i) % array::size(_data)];
}

/// Get value by index.
template<class T>
inline T const& Queue<T>::operator[](unsigned const i) const {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[(_head + i) % array::size(_data)];
}

namespace queue {

namespace {

template<class T>
void set_capacity(Queue<T>& q, u32_fast const new_capacity) {
	if (new_capacity <= q._size) {
		return;
	}
	u32_fast const old_capacity = array::size(q._data);
	array::resize(q._data, new_capacity);
	if (q._head + q._size > old_capacity) {
		// Push items from head to the end of the array
		u32_fast const to_end_count = old_capacity - q._head;
		std::memmove(
			array::begin(q._data) + new_capacity - to_end_count,
			array::begin(q._data) + q._head,
			to_end_count * sizeof(T)
		);
		q._head += new_capacity - old_capacity;
	}
}

template<class T>
void grow(Queue<T>& q, u32_fast const min_capacity = 0) {
	u32_fast new_capacity = array::size(q._data) * 2 + 8;
	if (new_capacity < min_capacity) {
		new_capacity = min_capacity;
	}
	set_capacity(q, new_capacity);
}

} // anonymous namespace

/**
	@addtogroup lib_core_queue
	@{
*/

/// Number of items.
template<class T>
inline u32_fast size(Queue<T> const& q) { return q._size; }

/// Number of items reserved.
template<class T>
inline u32_fast capacity(Queue<T> const& q) { return array::size(q._data); }

/// Number of items that can be pushed before the queue grows.
template<class T>
inline u32_fast space(Queue<T> const& q) { return array::size(q._data) - q._size; }

/// Whether there are any items in the queue.
template<class T>
inline bool any(Queue<T> const& q) { return q._size != 0; }

/// Whether there are no items in the queue.
template<class T>
inline bool empty(Queue<T> const& q) { return q._size == 0; }

/// Get first item.
template<class T>
inline T& front(Queue<T>& q) {
	return q[0];
}
/// Get first item.
template<class T>
inline T const& front(Queue<T> const& q) {
	return q[0];
}

/// Get last item.
template<class T>
inline T& back(Queue<T>& q) {
	TOGO_DEBUG_ASSERTE(any(q));
	return q[q._size - 1];
}
/// Get last item.
template<class T>
inline T const& back(Queue<T> const& q) {
	TOGO_DEBUG_ASSERTE(any(q));
	return q[q._size - 1];
}

/// Reserve at least new_capacity.
///
/// If new_capacity is less than the current size, this function
/// has no effect. Assume any references are invalid after call.
template<class T>
inline void reserve(Queue<T>& q, u32_fast const new_capacity) {
	set_capacity(q, new_capacity);
}

/// Remove all items.
template<class T>
inline void clear(Queue<T>& q) {
	q._head = 0;
	q._size = 0;
}

/// Add an item to the back of the queue.
template<class T>
inline T& push_back(Queue<T>& q, T const& item) {
	if (!space(q)) {
		grow(q);
	}
	return q[q._size++] = item;
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

/// Add an item to the front of the queue.
template<class T>
inline T& push_front(Queue<T>& q, T const& item) {
	if (!space(q)) {
		grow(q);
	}
	q._head = (q._head - 1 + array::size(q._data)) % array::size(q._data);
	++q._size;
	return q[0] = item;
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

/// Copy queue.
///
/// The allocator of dst is unchanged.
template<class T>
inline void copy(Queue<T>& dst, Queue<T> const& src) {
	array::copy(dst._data, src._data);
	dst._head = src._head;
	dst._size = src._size;
}

/** @} */ // end of doc-group lib_core_queue

} // namespace queue

} // namespace togo
