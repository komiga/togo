#line 2 "togo/priority_queue.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file priority_queue.hpp
@brief PriorityQueue interface.
@ingroup priority_queue
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/collection_types.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/array.hpp>

namespace togo {

/// Construct with allocator for storage.
///
/// PriorityQueue is empty and has null storage until it grows (either
/// explicitly through grow() or set_capacity() or by insertion).
template<class T>
inline PriorityQueue<T>::PriorityQueue(
	PriorityQueue<T>::less_func_type less_func,
	Allocator& allocator
)
	: _data(allocator)
	, _less_func(less_func)
{}

/// Access value by index.
template<class T>
inline T& PriorityQueue<T>::operator[](u32 const i) {
	return _data[i];
}

/// Access value by index.
template<class T>
inline T const& PriorityQueue<T>::operator[](u32 const i) const {
	return _data[i];
}

namespace priority_queue {

/**
	@addtogroup priority_queue
	@{
*/

/// Number of items.
template<class T>
inline u32 size(PriorityQueue<T> const& pq) { return array::size(pq._data); }

/// Number of items reserved.
template<class T>
inline u32 capacity(PriorityQueue<T> const& pq) { return array::capacity(pq._data); }

/// Number of items that can be added before a resize occurs.
template<class T>
inline u32 space(PriorityQueue<T> const& pq) { return array::space(pq._data); }

/// Returns true if there are any items in the queue.
template<class T>
inline bool any(PriorityQueue<T> const& pq) { return array::any(pq._data); }

/// Returns true if there are no items in the queue.
template<class T>
inline bool empty(PriorityQueue<T> const& pq) { return array::empty(pq._data); }

/// Beginning iterator: [begin, end).
template<class T>
inline T* begin(PriorityQueue<T>& pq) {
	return array::begin(pq._data);
}
/// Beginning iterator: [begin, end).
template<class T>
inline T const* begin(PriorityQueue<T> const& pq) {
	return array::begin(pq._data);
}

/// Ending iterator: [begin, end).
template<class T>
inline T* end(PriorityQueue<T>& pq) {
	return array::end(pq._data);
}
/// Ending iterator: [begin, end).
template<class T>
inline T const* end(PriorityQueue<T> const& pq) {
	return array::end(pq._data);
}

/// Access first item (maximum).
template<class T>
inline T& front(PriorityQueue<T>& pq) {
	TOGO_DEBUG_ASSERTE(any(pq));
	return array::front(pq._data);
}
/// Access first item (maximum).
template<class T>
inline T const& front(PriorityQueue<T> const& pq) {
	TOGO_DEBUG_ASSERTE(any(pq));
	return array::front(pq._data);
}

/// Access last item.
template<class T>
inline T& back(PriorityQueue<T>& pq) {
	TOGO_DEBUG_ASSERTE(any(pq));
	return array::back(pq._data);
}
/// Access last item.
template<class T>
inline T const& back(PriorityQueue<T> const& pq) {
	TOGO_DEBUG_ASSERTE(any(pq));
	return array::back(pq._data);
}

/// Shrink capacity to fit current size.
template<class T>
inline void shrink_to_fit(PriorityQueue<T>& pq) {
	array::shrink_to_fit(pq._data);
}

/// Reserve at least new_capacity.
template<class T>
inline void reserve(PriorityQueue<T>& pq, u32 const new_capacity) {
	array::reserve(pq._data, new_capacity);
}

/// Remove all items.
template<class T>
inline void clear(PriorityQueue<T>& pq) {
	array::clear(pq._data);
}

namespace {

// Children of i are at i * 2. Parent of i is at i / 2.
// These functions take [1..size(pq)] indices for mathematical
// niceness (notice the i - 1 in item access)

template<class T>
inline bool less(PriorityQueue<T> const& pq, u32 const ix, u32 const iy) {
	return pq._less_func(pq[ix - 1], pq[iy - 1]);
}

template<class T>
inline bool swap_if_less(PriorityQueue<T>& pq, u32 const ix, u32 const iy) {
	T& x = pq[ix - 1];
	T& y = pq[iy - 1];
	if (pq._less_func(x, y)) {
		togo::swap(x, y);
		return true;
	}
	return false;
}

template<class T>
inline void sink(PriorityQueue<T>& pq, u32 i) {
	u32 child;
	while ((child = i * 2) <= size(pq)) {
		// Swap only with the largest child
		if (child < size(pq) && less(pq, child, child + 1)) {
			++child;
		}
		if (!swap_if_less(pq, i, child)) {
			break;
		}
		i = child;
	}
}

template<class T>
inline void swim(PriorityQueue<T>& pq, u32 i) {
	u32 parent = i / 2;
	while (i > 1 && swap_if_less(pq, parent, i)) {
		i = parent;
		parent /= 2;
	}
}

template<class T>
inline void grow(PriorityQueue<T>& pq) {
	array::grow(pq._data);
}

} // anonymous namespace

/// Add an item.
///
/// The queue will grow if needed.
template<class T>
inline void push(PriorityQueue<T>& pq, T const& item) {
	array::push_back(pq._data, item);
	swim(pq, size(pq));
}

/// Remove the largest item.
template<class T>
inline void pop(PriorityQueue<T>& pq) {
	TOGO_DEBUG_ASSERTE(any(pq));
	if (size(pq) > 1) {
		swap(front(pq), back(pq));
	}
	array::pop_back(pq._data);
	sink(pq, 1);
}

/** @} */ // end of doc-group priority_queue

} // namespace priority_queue

/** @cond INTERNAL */

// ADL support

template<class T>
inline T* begin(PriorityQueue<T>& pq) {
	return priority_queue::begin(pq);
}
template<class T>
inline T const* begin(PriorityQueue<T> const& pq) {
	return priority_queue::begin(pq);
}

template<class T>
inline T* end(PriorityQueue<T>& pq) {
	return priority_queue::end(pq);
}
template<class T>
inline T const* end(PriorityQueue<T> const& pq) {
	return priority_queue::end(pq);
}

/** @endcond */ // INTERNAL

} // namespace togo
