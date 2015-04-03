#line 2 "togo/core/collection/hash_map.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief HashMap interface.
@ingroup lib_core_collection
@ingroup lib_core_hash_map

@defgroup lib_core_hash_map HashMap
@ingroup lib_core_collection
@details

@note Iterator access is unordered.

@par
@note Capacity represents only 0.7 (the load factor) of partition storage.
When reserving, <code>new_capacity / 0.7</code> space is stored to preserve
the partition invariant.
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/collection/array.hpp>

namespace togo {

/// Construct with allocator for storage.
template<class K, class T>
inline HashMap<K, T>::HashMap(Allocator& allocator)
	: _head(allocator)
	, _data(allocator)
{}

namespace hash_map {

/**
	@addtogroup lib_core_hash_map
	@{
*/

/** @cond INTERNAL */
#define TOGO_HASH_MAP_MAX_LOAD 0.70f
/** @endcond */ // INTERNAL

/// Number of partitions.
template<class K, class T>
inline u32_fast num_partitions(HashMap<K, T> const& hm) { return array::size(hm._head); }

/// Number of values.
template<class K, class T>
inline u32_fast size(HashMap<K, T> const& hm) { return array::size(hm._data); }

/// Number of values reserved.
template<class K, class T>
inline u32_fast capacity(HashMap<K, T> const& hm) {
	return static_cast<u32_fast>(num_partitions(hm) * TOGO_HASH_MAP_MAX_LOAD);
}

/// Number of values that can be added before a resize occurs.
template<class K, class T>
inline u32_fast space(HashMap<K, T> const& hm) {
	return capacity(hm) - size(hm);
}

/// Whether there are any values.
template<class K, class T>
inline bool any(HashMap<K, T> const& hm) { return size(hm) != 0; }

/// Whether there are no values.
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
	FindData const fd = internal::find(hm, key);
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
		FindData const last = internal::find(hm, &last_node);
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
	hm = rvalue_ref(new_hm);
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
/// If key does not exist, it will be inserted.
template<class K, class T>
inline T& set(HashMap<K, T>& hm, K const key, T const& value) {
	if (!space(hm)) {
		internal::grow(hm);
	}
	auto const index = internal::make(hm, key, true);
	return hm._data[index].value = value;
}

/// Add item.
///
/// If there are existing values with key, they are retained.
template<class K, class T>
inline T& push(HashMap<K, T>& hm, K const key, T const& value) {
	if (!space(hm)) {
		internal::grow(hm);
	}
	auto const index = internal::make(hm, key, false);
	return hm._data[index].value = value;
}

/// Find first item with key.
///
/// If key does not exist, nullptr will be returned.
/// If multiple values are assigned to key, this will get the most
/// recently pushed one.
template<class K, class T>
inline T* find(HashMap<K, T>& hm, K const key) {
	auto const index = internal::find(hm, key).data;
	return (index != END) ? &hm._data[index].value : nullptr;
}

/// Find first item with key.
///
/// If key does not exist, nullptr will be returned.
/// If multiple values are assigned to key, this will get the most
/// recently pushed one.
template<class K, class T>
inline T const* find(HashMap<K, T> const& hm, K const key) {
	auto const index = internal::find(hm, key).data;
	return (index != END) ? &hm._data[index].value : nullptr;
}

/// Get first node with key.
///
/// If there are no items with key, nullptr will be returned.
template<class K, class T>
inline HashMapNode<K, T>* find_node(
	HashMap<K, T>& hm,
	K const key
) {
	auto const index = internal::find(hm, key).data;
	return (index != END) ? &hm._data[index] : nullptr;
}

/// Get first node with key.
///
/// If there are no items with key, nullptr will be returned.
template<class K, class T>
inline HashMapNode<K, T> const* find_node(
	HashMap<K, T> const& hm,
	K const key
) {
	auto const index = internal::find(hm, key).data;
	return (index != END) ? &hm._data[index] : nullptr;
}

/// Get next node in keyset.
///
/// An assertion will fail if node is nullptr. Returns nullptr when
/// there are no more nodes for the key.
template<class K, class T>
inline HashMapNode<K, T>* next_node(
	HashMap<K, T>& hm,
	HashMapNode<K, T>* node
) {
	TOGO_ASSERTE(node != nullptr);
	K const key = node->key;
	while (node->next != END) {
		node = &hm._data[node->next];
		if (node->key == key) {
			return node;
		}
	}
	return nullptr;
}

/// Get next node in keyset.
///
/// An assertion will fail if node is nullptr. Returns nullptr when
/// there are no more nodes for the key.
template<class K, class T>
inline HashMapNode<K, T> const* next_node(
	HashMap<K, T> const& hm,
	HashMapNode<K, T> const* node
) {
	TOGO_ASSERTE(node != nullptr);
	K const key = node->key;
	while (node->next != END) {
		node = &hm._data[node->next];
		if (node->key == key) {
			return node;
		}
	}
	return nullptr;
}

/// Whether there is a value with key.
template<class K, class T>
inline bool has(HashMap<K, T> const& hm, K const key) {
	return internal::find(hm, key).data != END;
}

/// Number of values with key.
template<class K, class T>
inline unsigned count(HashMap<K, T> const& hm, K const key) {
	auto const* node = find_node(hm, key);
	unsigned count = 0;
	while (node != nullptr) {
		node = next_node(hm, node);
		++count;
	}
	return count;
}

/// Remove value.
///
/// If there are multiple values with key, the most recently added
/// one is removed.
template<class K, class T>
inline void remove(HashMap<K, T>& hm, K const key) {
	FindData const fd = internal::find(hm, key);
	if (fd.data != END) {
		internal::remove(hm, fd);
	}
}

/// Remove node.
///
/// An assertion will fail if node is nullptr.
template<class K, class T>
inline void remove(HashMap<K, T>& hm, HashMapNode<K, T> const* node) {
	TOGO_ASSERTE(node != nullptr);
	FindData const fd = internal::find(hm, node);
	if (fd.data != END) {
		internal::remove(hm, fd);
	}
}

/** @cond INTERNAL */
#undef TOGO_HASH_MAP_MAX_LOAD
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group lib_core_hash_map

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
