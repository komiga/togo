#line 2 "togo/array.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Array interface.
@ingroup collections
@ingroup array
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/collection_types.hpp>
#include <togo/assert.hpp>
#include <togo/memory.hpp>

#include <cstring>

namespace togo {

template<class T>
inline Array<T>::~Array() {
	_allocator->deallocate(_data);
}

/// Construct with allocator for storage.
///
/// Array is empty and has null storage until it grows (either
/// explicitly through grow() or set_capacity() or by insertion).
template<class T>
inline Array<T>::Array(Allocator& allocator)
	: _size(0)
	, _capacity(0)
	, _data(nullptr)
	, _allocator(&allocator)
{}

// Move constructor.
template<class T>
inline Array<T>::Array(Array<T>&& other)
	: _size(other._size)
	, _capacity(other._capacity)
	, _data(other._data)
	, _allocator(other._allocator)
{
	other._size = 0;
	other._capacity = 0;
	other._data = nullptr;
}

// Move assignment operator.
template<class T>
inline Array<T>& Array<T>::operator=(Array<T>&& other) {
	_allocator->deallocate(_data);
	_size = other._size;
	_capacity = other._capacity;
	_data = other._data;
	_allocator = other._allocator;
	other._size = 0;
	other._capacity = 0;
	other._data = nullptr;
	return *this;
}

/// Access value by index.
template<class T>
inline T& Array<T>::operator[](unsigned const i) {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

/// Access value by index.
template<class T>
inline T const& Array<T>::operator[](unsigned const i) const {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

namespace array {

/**
	@addtogroup array
	@{
*/

/// Number of items.
template<class T>
inline u32_fast size(Array<T> const& a) { return a._size; }

/// Number of items reserved.
template<class T>
inline u32_fast capacity(Array<T> const& a) { return a._capacity; }

/// Number of items that can be added before a resize occurs.
template<class T>
inline u32_fast space(Array<T> const& a) { return a._capacity - a._size; }

/// Returns true if there are any items in the array.
template<class T>
inline bool any(Array<T> const& a) { return a._size != 0; }

/// Returns true if there are no items in the array.
template<class T>
inline bool empty(Array<T> const& a) { return a._size == 0; }

/// Beginning iterator: [begin, end).
template<class T>
inline T* begin(Array<T>& a) {
	return a._data;
}
/// Beginning iterator: [begin, end).
template<class T>
inline T const* begin(Array<T> const& a) {
	return a._data;
}

/// Ending iterator: [begin, end).
template<class T>
inline T* end(Array<T>& a) {
	return a._data + a._size;
}
/// Ending iterator: [begin, end).
template<class T>
inline T const* end(Array<T> const& a) {
	return a._data + a._size;
}

/// Access first item.
template<class T>
inline T& front(Array<T>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}
/// Access first item.
template<class T>
inline T const& front(Array<T> const& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}

/// Access last item.
template<class T>
inline T& back(Array<T>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[a._size - 1];
}
/// Access last item.
template<class T>
inline T const& back(Array<T> const& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[a._size - 1];
}

/// Change buffer size.
///
/// If new_capacity is lower than the size, the array is resized
/// to new_capacity.
template<class T>
void set_capacity(Array<T>& a, u32_fast const new_capacity) {
	if (new_capacity == a._capacity) {
		return;
	}

	// Effectively a resize
	if (new_capacity < a._size) {
		a._size = new_capacity;
	}

	T* new_data = nullptr;
	if (new_capacity != 0) {
		new_data = static_cast<T*>(
			a._allocator->allocate(new_capacity * sizeof(T), alignof(T))
		);
		if (a._data) {
			std::memcpy(new_data, a._data, a._size * sizeof(T));
		}
	}
	a._allocator->deallocate(a._data);
	a._data = new_data;
	a._capacity = new_capacity;
}

/// Grow the array with a doubling factor.
///
/// Cost of insertion should be amortized O(1), assuming no aggressive
/// shrinking. Grows to at least min_capacity if it is non-zero.
template<class T>
inline void grow(Array<T>& a, u32_fast const min_capacity = 0) {
	u32_fast new_capacity = a._capacity * 2 + 8;
	if (min_capacity > new_capacity) {
		new_capacity = min_capacity;
	}
	set_capacity(a, new_capacity);
}

/// Shrink capacity to fit current size.
template<class T>
inline void shrink_to_fit(Array<T>& a) {
	if (a._capacity != a._size) {
		set_capacity(a._size);
	}
}

/// Change size.
///
/// Upsize grows by using new_size as the minimum capacity.
template<class T>
inline void resize(Array<T>& a, u32_fast const new_size) {
	if (new_size > a._capacity) {
		grow(a, new_size);
	}
	a._size = new_size;
}

/// Reserve at least new_capacity.
template<class T>
inline void reserve(Array<T>& a, u32_fast const new_capacity) {
	if (new_capacity > a._capacity) {
		set_capacity(a, new_capacity);
	}
}

/// Remove all items.
template<class T>
inline void clear(Array<T>& a) {
	resize(a, 0);
}

/// Add an item to the end. The array will grow if needed.
template<class T>
inline T& push_back(Array<T>& a, T const& item) {
	if (!space(a)) {
		grow(a);
	}
	return a._data[a._size++] = item;
}

/// Remove the last item.
template<class T>
inline void pop_back(Array<T>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	--a._size;
}

/// Remove an item by index.
template<class T>
inline void remove(Array<T>& a, unsigned const i) {
	TOGO_DEBUG_ASSERTE(any(a));
	TOGO_DEBUG_ASSERTE(i < a._size);
	--a._size;
	if (i < a._size) {
		std::memmove(
			a._data + i, a._data + i + 1,
			(a._size - i) * sizeof(T)
		);
	}
}

/// Remove an item by address.
///
/// If ptr is nullptr, an assertion will fail.
template<class T>
inline void remove(Array<T>& a, T const* const ptr) {
	TOGO_ASSERTE(ptr != nullptr);
	TOGO_DEBUG_ASSERTE(any(a));
	TOGO_DEBUG_ASSERTE(array::begin(a) <= ptr && ptr < array::end(a));
	remove(a, (ptr - a._data) / sizeof(T));
}

/// Copy array.
///
/// The allocator of dst is unchanged.
/// Existing capacity in dst will be retained.
template<class T>
inline void copy(Array<T>& dst, Array<T> const& src) {
	// If resize would incur memcpy(), prevent it
	if (src._size > dst._capacity) {
		dst._allocator->deallocate(dst._data);
	}
	resize(dst, src._size);
	std::memcpy(dst._data, src._data, src._size * sizeof(T));
}

/** @} */ // end of doc-group array

} // namespace array

/** @cond INTERNAL */

// ADL support

template<class T>
inline T* begin(Array<T>& a) {
	return array::begin(a);
}
template<class T>
inline T const* begin(Array<T> const& a) {
	return array::begin(a);
}

template<class T>
inline T* end(Array<T>& a) {
	return array::end(a);
}
template<class T>
inline T const* end(Array<T> const& a) {
	return array::end(a);
}

/** @endcond */ // INTERNAL

} // namespace togo
