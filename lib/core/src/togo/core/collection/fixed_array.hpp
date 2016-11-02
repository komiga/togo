#line 2 "togo/core/collection/fixed_array.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief FixedArray interface.
@ingroup lib_core_collection
@ingroup lib_core_fixed_array

@defgroup lib_core_fixed_array FixedArray
@ingroup lib_core_collection
@details
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/types.hpp>

#include <cstring>
#include <initializer_list>

namespace togo {
namespace fixed_array {

/**
	@addtogroup lib_core_fixed_array
	@{
*/

/// Construct.
template<class T, unsigned N>
inline FixedArray<T, N>::FixedArray()
	: _size(0)
	, _data()
{}

/// Construct to array.
template<class T, unsigned N>
inline FixedArray<T, N>::FixedArray(std::initializer_list<T> const ilist)
	: _size(end(ilist) - begin(ilist))
	, _data()
{
	TOGO_ASSERTE(_size <= N);
	std::memcpy(_data, begin(ilist), _size * sizeof(T));
}

/// Get value by index.
template<class T, unsigned N>
inline T& FixedArray<T, N>::operator[](unsigned const i) {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

/// Get value by index.
template<class T, unsigned N>
inline T const& FixedArray<T, N>::operator[](unsigned const i) const {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

/// Number of items.
template<class T, unsigned N>
inline u32_fast size(FixedArray<T, N> const& a) { return a._size; }

/// Number of items reserved.
template<class T, unsigned N>
inline u32_fast capacity(FixedArray<T, N> const& /*a*/) { return N; }

/// Number of items that can be added before the array is full.
template<class T, unsigned N>
inline u32_fast space(FixedArray<T, N> const& a) { return N - a._size; }

/// Whether there are any items in the array.
template<class T, unsigned N>
inline bool any(FixedArray<T, N> const& a) { return a._size != 0; }

/// Whether there are no items in the array.
template<class T, unsigned N>
inline bool empty(FixedArray<T, N> const& a) { return a._size == 0; }

/// Beginning iterator: [begin, end).
template<class T, unsigned N>
inline T* begin(FixedArray<T, N>& a) {
	return a._data;
}
/// Beginning iterator: [begin, end).
template<class T, unsigned N>
inline T const* begin(FixedArray<T, N> const& a) {
	return a._data;
}

/// Ending iterator: [begin, end).
template<class T, unsigned N>
inline T* end(FixedArray<T, N>& a) {
	return a._data + a._size;
}
/// Ending iterator: [begin, end).
template<class T, unsigned N>
inline T const* end(FixedArray<T, N> const& a) {
	return a._data + a._size;
}

/// Get first item.
template<class T, unsigned N>
inline T& front(FixedArray<T, N>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}
/// Get first item.
template<class T, unsigned N>
inline T const& front(FixedArray<T, N> const& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}

/// Get last item.
template<class T, unsigned N>
inline T& back(FixedArray<T, N>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[a._size - 1];
}
/// Get last item.
template<class T, unsigned N>
inline T const& back(FixedArray<T, N> const& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[a._size - 1];
}

/// Change size.
///
/// An assertion will fail if new_size is larger than the capacity.
template<class T, unsigned N>
inline void resize(FixedArray<T, N>& a, u32_fast const new_size) {
	TOGO_ASSERTE(new_size <= capacity(a));
	a._size = new_size;
}

/// Increase size.
///
/// An assertion will fail if new_size is larger than the capacity.
template<class T, unsigned N>
inline void increase_size(FixedArray<T, N>& a, u32_fast const num_items) {
	resize(a, size(a) + num_items);
}

/// Remove all items.
template<class T, unsigned N>
inline void clear(FixedArray<T, N>& a) {
	resize(a, 0);
}

/// Add an item to the end.
///
/// An assertion will fail if the array is full.
template<class T, unsigned N>
inline T& push_back(FixedArray<T, N>& a, T const& item) {
	TOGO_DEBUG_ASSERTE(space(a));
	return a._data[a._size++] = item;
}

/// Remove the last item.
///
/// An assertion will fail if the array is empty.
template<class T, unsigned N>
inline void pop_back(FixedArray<T, N>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	--a._size;
}

/// Copy array.
template<class T, unsigned N, unsigned M>
inline void copy(FixedArray<T, N>& dst, FixedArray<T, M> const& src) {
	resize(dst, src._size);
	std::memcpy(dst._data, src._data, src._size * sizeof(T));
}

/** @} */ // end of doc-group lib_core_fixed_array

} // namespace fixed_array
} // namespace togo
