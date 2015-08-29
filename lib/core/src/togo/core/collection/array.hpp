#line 2 "togo/core/collection/array.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Array interface.
@ingroup lib_core_collection
@ingroup lib_core_array

@defgroup lib_core_array Array
@ingroup lib_core_collection
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/collection/npod.hpp>

#include <cstring>

namespace togo {
namespace array {

/**
	@addtogroup lib_core_array
	@{
*/

template<class T>
inline Array<T>::~Array() {
	collection_npod_destruct(_data, _data + _size);
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
{
	TOGO_CONSTRAIN_POD_COLLECTION(T);
}

/// Move constructor.
template<class T>
inline Array<T>::Array(Array<T>&& other)
	: _size(other._size)
	, _capacity(other._capacity)
	, _data(other._data)
	, _allocator(other._allocator)
{
	TOGO_CONSTRAIN_POD_COLLECTION(T);
	other._size = 0;
	other._capacity = 0;
	other._data = nullptr;
}

/// Move assignment operator.
template<class T>
inline Array<T>& Array<T>::operator=(Array<T>&& other) {
	collection_npod_destruct(_data, _data + _size);
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

/// Get value by index.
template<class T>
inline T& Array<T>::operator[](unsigned const i) {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

/// Get value by index.
template<class T>
inline T const& Array<T>::operator[](unsigned const i) const {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

/// Convert to array reference.
template<class T>
inline Array<T>::operator ArrayRef<T>() const {
	return array_ref(_size, _data);
}

/// Convert to array reference.
template<class T>
inline Array<T>::operator ArrayRef<T const>() const {
	return array_ref(_size, _data);
}

/// Number of items.
template<class T>
inline u32_fast size(Array<T> const& a) { return a._size; }

/// Number of items reserved.
template<class T>
inline u32_fast capacity(Array<T> const& a) { return a._capacity; }

/// Number of items that can be added before a resize occurs.
template<class T>
inline u32_fast space(Array<T> const& a) { return a._capacity - a._size; }

/// Whether there are any items in the array.
template<class T>
inline bool any(Array<T> const& a) { return a._size != 0; }

/// Whether there are no items in the array.
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

/// Get first item.
template<class T>
inline T& front(Array<T>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}
/// Get first item.
template<class T>
inline T const& front(Array<T> const& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}

/// Get last item.
template<class T>
inline T& back(Array<T>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[a._size - 1];
}
/// Get last item.
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
		collection_npod_destruct(a._data + new_capacity, a._data + a._size);
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

/// Grow the array capacity with a doubling factor.
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
	if (new_size > a._size) {
		collection_npod_construct(a._data + a._size, a._data + new_size);
	} else if (new_size < a._size) {
		collection_npod_destruct(a._data + new_size, a._data + a._size);
	}
	a._size = new_size;
}

/// Increase size.
template<class T>
inline void increase_size(Array<T>& a, u32_fast const num_items) {
	resize(a, size(a) + num_items);
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

/// Add an item to the end (in-place construction). The array will grow if needed.
template<class T, class... P>
inline T& push_back_inplace(Array<T>& a, P&&... init) {
	if (!space(a)) {
		grow(a);
	}
	new(a._data + a._size++) T(forward<P>(init)...);
	return back(a);
}

/// Remove the last item.
template<class T>
inline void pop_back(Array<T>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	--a._size;
	collection_npod_destruct(a._data + a._size, a._data + a._size + 1);
}

/// Remove an item by index.
///
/// Moves all subsequent elements left in memory.
template<class T>
inline void remove(Array<T>& a, unsigned const i) {
	TOGO_DEBUG_ASSERTE(any(a));
	TOGO_DEBUG_ASSERTE(i < a._size);
	--a._size;
	collection_npod_destruct(a._data + i, a._data + i + 1);
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

/// Remove-overwrite an item by index.
///
/// Assigns the specified element with the last element instead of moving
/// all subsequent elements.
template<class T>
inline void remove_over(Array<T>& a, unsigned const i) {
	TOGO_DEBUG_ASSERTE(any(a));
	TOGO_DEBUG_ASSERTE(i < a._size);
	--a._size;
	collection_npod_remove_over(a._data, a._size, i);
}

/// Remove-overwrite an item by address.
///
/// If ptr is nullptr, an assertion will fail.
template<class T>
inline void remove_over(Array<T>& a, T const* const ptr) {
	TOGO_ASSERTE(ptr != nullptr);
	TOGO_DEBUG_ASSERTE(any(a));
	TOGO_DEBUG_ASSERTE(array::begin(a) <= ptr && ptr < array::end(a));
	remove_over(a, (ptr - a._data) / sizeof(T));
}

/// Copy array.
///
/// The allocator of dst is unchanged.
/// Existing capacity in dst will be retained.
template<class T>
inline void copy(Array<T>& dst, Array<T> const& src) {
	// If resize would incur memcpy(), prevent it
	if (src._size > dst._capacity) {
		set_capacity(dst, 0);
	}
	reserve(dst, src._size);
	collection_npod_copy(dst._data, dst._size, src._data, src._size);
	dst._size = src._size;
}

/** @} */ // end of doc-group lib_core_array

} // namespace array
} // namespace togo
