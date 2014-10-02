#line 2 "togo/hash_map.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file hash_map.hpp
@brief HashMap interface.
@ingroup collections
@ingroup hash_map
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/collection_types.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/memory.hpp>
#include <togo/array.hpp>

#include <utility>

namespace togo {

/// Construct with allocator for storage.
template<class K, class T>
inline HashMap<K, T>::HashMap(Allocator& allocator)
	: _head(allocator)
	, _data(allocator)
{}

namespace hash_map {

/**
	@addtogroup hash_map
	@{
*/

/** @cond INTERNAL */
#define TOGO_HASH_MAP_MAX_LOAD 0.70f
/** @endcond */ // INTERNAL

/// Number of partitions.
template<class K, class T>
inline u32_fast num_partitions(HashMap<K, T> const& hm) { return array::size(hm._head); }

/// Number of items.
template<class K, class T>
inline u32_fast size(HashMap<K, T> const& hm) { return array::size(hm._data); }

/// Number of items reserved.
template<class K, class T>
inline u32_fast capacity(HashMap<K, T> const& hm) {
	return static_cast<u32_fast>(num_partitions(hm) * TOGO_HASH_MAP_MAX_LOAD);
}

/// Number of items that can be added before a resize occurs.
template<class K, class T>
inline u32_fast space(HashMap<K, T> const& hm) {
	return capacity(hm) - size(hm);
}

/// Returns true if there are any items in the array.
template<class K, class T>
inline bool any(HashMap<K, T> const& hm) { return size(hm) != 0; }

/// Returns true if there are no items in the array.
template<class K, class T>
inline bool empty(HashMap<K, T> const& hm) { return size(hm) == 0; }

/// Beginning iterator: [begin, end).
template<class K, class T>
inline HashMapNode<K, T>* begin(HashMap<K, T>& hm) {
	return array::begin(hm._data);
}
/// Beginning iterator: [begin, end).
template<class K, class T>
inline HashMapNode<K, T> const* begin(HashMap<K, T> const& hm) {
	return array::begin(hm._data);
}

/// Ending iterator: [begin, end).
template<class K, class T>
inline HashMapNode<K, T>* end(HashMap<K, T>& hm) {
	return array::end(hm._data);
}
/// Ending iterator: [begin, end).
template<class K, class T>
inline HashMapNode<K, T> const* end(HashMap<K, T> const& hm) {
	return array::end(hm._data);
}

/** @cond INTERNAL */
enum : unsigned {
	END = ~0u,
};

struct FindData {
	// Index into _head
	u32 head;
	// Index into _data
	u32 data;
	// Previous node with same key
	u32 data_prev;
};

namespace internal {

template<class K, class T>
FindData find(HashMap<K, T> const& hm, K const key) {
	FindData fd{END, END, END};
	if (array::empty(hm._head)) {
		return fd;
	}

	fd.head = key % num_partitions(hm);
	fd.data = hm._head[fd.head];
	while (fd.data != END) {
		auto const& node = hm._data[fd.data];
		if (node.key == key) {
			break;
		}
		fd.data_prev = fd.data;
		fd.data = node.next;
	}
	return fd;
}

template<class K, class T>
FindData find(
	HashMap<K, T> const& hm,
	HashMapNode<K, T> const* node
) {
	FindData fd{END, END, END};
	if (array::empty(hm._head)) {
		return fd;
	}

	fd.head = node->key % num_partitions(hm);
	fd.data = hm._head[fd.head];
	while (fd.data != END) {
		auto const& it_node = hm._data[fd.data];
		if (&it_node == node) {
			break;
		}
		fd.data_prev = fd.data;
		fd.data = it_node.next;
	}
	return fd;
}

template<class K, class T>
u32 make(HashMap<K, T>& hm, K const key, bool const keep) {
	TOGO_DEBUG_ASSERTE(array::any(hm._head));
	FindData const fd = find(hm, key);
	if (keep && fd.data != END) {
		return fd.data;
	}
	u32 const index = array::size(hm._data);
	HashMapNode<K, T> node;
	node.key = key;
	node.next = fd.data;
	array::push_back(hm._data, node);
	if (fd.data_prev == END) {
		hm._head[fd.head] = index;
	} else {
		hm._data[fd.data_prev].next = index;
	}
	return index;
}

template<class K, class T>
void remove(HashMap<K, T>& hm, FindData const& fd) {
	if (fd.data_prev == END) {
		// find() was immediate hit; fd is the first item
		// in the head. Move the head to the next item (if any).
		hm._head[fd.head] = hm._data[fd.data].next;
	} else {
		hm._data[fd.data_prev].next = hm._data[fd.data].next;
	}

	// Move last node to the removed position
	if (fd.data != array::size(hm._data) - 1) {
		auto const& last_node = array::back(hm._data);
		FindData const last = find(hm, &last_node);
		if (last.data_prev == END) {
			hm._head[last.head] = fd.data;
		} else {
			hm._data[last.data_prev].next = fd.data;
		}
		hm._data[fd.data] = last_node;
	}
	array::pop_back(hm._data);
}

template<class K, class T>
inline void remove_key(HashMap<K, T>& hm, K const key) {
	FindData const fd = find(hm, key);
	if (fd.data != END) {
		remove(hm, fd);
	}
}

template<class K, class T>
void resize(HashMap<K, T>& hm, u32_fast const new_size) {
	HashMap<K, T> new_hm{*hm._head._allocator};
	array::resize(new_hm._head, new_size);
	array::reserve(new_hm._data, min(new_size, array::capacity(hm._data)));
	for (unsigned i = 0; i < new_size; ++i) {
		new_hm._head[i] = END;
	}
	u32 index;
	for (auto const& node : hm._data) {
		index = make(hm, node.key, false);
		hm._data[index].value = node.value;
	}
	hm = std::move(new_hm);
}

template<class K, class T>
inline void grow(HashMap<K, T>& hm) {
	resize(hm, num_partitions(hm) * 2 + 16);
}

} // namespace internal
/** @endcond */ // INTERNAL

/// Reserve at least new_capacity.
template<class K, class T>
inline void reserve(HashMap<K, T>& hm, u32_fast const new_capacity) {
	if (new_capacity > capacity(hm)) {
		internal::resize(hm, (new_capacity / TOGO_HASH_MAP_MAX_LOAD) + 1);
	}
}

/// Remove all items.
template<class K, class T>
inline void clear(HashMap<K, T>& hm) {
	auto const size = num_partitions(hm);
	for (unsigned i = 0; i < size; ++i) {
		hm._head[i] = END;
	}
	array::clear(hm._data);
}

/// Set item.
///
/// If item does not exist, it will be inserted.
template<class K, class T>
inline void set(HashMap<K, T>& hm, K const key, T const& value) {
	if (!space(hm)) {
		internal::grow(hm);
	}
	auto const index = internal::make(hm, key, true);
	hm._data[index].value = value;
}

/// Get item.
///
/// If item does not exist, nullptr will be returned.
template<class K, class T>
inline T* get(HashMap<K, T>& hm, K const key) {
	auto const index = internal::find(hm, key).data;
	return (index != END) ? &hm._data[index].value : nullptr;
}

/// Get item.
///
/// If item does not exist, nullptr will be returned.
template<class K, class T>
inline T const* get(HashMap<K, T> const& hm, K const key) {
	auto const index = internal::find(hm, key).data;
	return (index != END) ? &hm._data[index].value : nullptr;
}

/// Check if item exists.
template<class K, class T>
inline bool has(HashMap<K, T> const& hm, K const key) {
	return internal::find(hm, key).data != END;
}

/// Remove item.
template<class K, class T>
inline void remove(HashMap<K, T>& hm, K const key) {
	internal::remove_key(hm, key);
}

/** @cond INTERNAL */
#undef TOGO_HASH_MAP_MAX_LOAD
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group hash_map

} // namespace hash_map

/** @cond INTERNAL */

// ADL support

template<class K, class T>
inline HashMapNode<K, T>* begin(HashMap<K, T>& hm) {
	return hash_map::begin(hm);
}
template<class K, class T>
inline HashMapNode<K, T> const* begin(HashMap<K, T> const& hm) {
	return hash_map::begin(hm);
}

template<class K, class T>
inline HashMapNode<K, T>* end(HashMap<K, T>& hm) {
	return hash_map::end(hm);
}
template<class K, class T>
inline HashMapNode<K, T> const* end(HashMap<K, T> const& hm) {
	return hash_map::end(hm);
}

/** @endcond */ // INTERNAL

} // namespace togo
