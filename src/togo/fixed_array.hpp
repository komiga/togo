#line 2 "togo/fixed_array.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file fixed_array.hpp
@brief FixedArray interface.
@ingroup collections
@ingroup fixed_array
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/collection_types.hpp>
#include <togo/memory.hpp>
#include <togo/assert.hpp>

#include <cstring>

namespace togo {

/// Construct.
template<class T, unsigned N>
inline FixedArray<T, N>::FixedArray()
	: _size(0)
	, _data()
{}

/// Access value by index.
template<class T, unsigned N>
inline T& FixedArray<T, N>::operator[](unsigned const i) {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

/// Access value by index.
template<class T, unsigned N>
inline T const& FixedArray<T, N>::operator[](unsigned const i) const {
	TOGO_DEBUG_ASSERTE(i < _size);
	return _data[i];
}

namespace fixed_array {

/**
	@addtogroup fixed_array
	@{
*/

/// Number of items.
template<class T, unsigned N>
inline u32_fast size(FixedArray<T, N> const& a) { return a._size; }

/// Number of items reserved.
template<class T, unsigned N>
inline u32_fast capacity(FixedArray<T, N> const& /*a*/) { return N; }

/// Number of items that can be added before the array is full.
template<class T, unsigned N>
inline u32_fast space(FixedArray<T, N> const& a) { return N - a._size; }

/// Returns true if there are any items in the array.
template<class T, unsigned N>
inline bool any(FixedArray<T, N> const& a) { return a._size != 0; }

/// Returns true if there are no items in the array.
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

/// Access first item.
template<class T, unsigned N>
inline T& front(FixedArray<T, N>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}
/// Access first item.
template<class T, unsigned N>
inline T const& front(FixedArray<T, N> const& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[0];
}

/// Access last item.
template<class T, unsigned N>
inline T& back(FixedArray<T, N>& a) {
	TOGO_DEBUG_ASSERTE(any(a));
	return a._data[a._size - 1];
}
/// Access last item.
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

/// Remove all items.
template<class T, unsigned N>
inline void clear(FixedArray<T, N>& a) {
	resize(a, 0);
}

/// Add an item to the end.
///
/// An assertion will fail if the array is full.
template<class T, unsigned N>
inline void push_back(FixedArray<T, N>& a, T const& item) {
	TOGO_DEBUG_ASSERTE(space(a));
	a._data[a._size++] = item;
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

/** @} */ // end of doc-group fixed_array

} // namespace fixed_array

/** @cond INTERNAL */

// ADL support

template<class T, unsigned N>
inline T* begin(FixedArray<T, N>& a) {
	return fixed_array::begin(a);
}
template<class T, unsigned N>
inline T const* begin(FixedArray<T, N> const& a) {
	return fixed_array::begin(a);
}

template<class T, unsigned N>
inline T* end(FixedArray<T, N>& a) {
	return fixed_array::end(a);
}
template<class T, unsigned N>
inline T const* end(FixedArray<T, N> const& a) {
	return fixed_array::end(a);
}

/** @endcond */ // INTERNAL

} // namespace togo
