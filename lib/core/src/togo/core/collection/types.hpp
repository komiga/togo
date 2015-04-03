#line 2 "togo/core/collection/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Collection types.
@ingroup lib_core_types
@ingroup lib_core_collection
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/constraints.hpp>
#include <togo/core/memory/types.hpp>

namespace togo {

// Forward declarations
template<class T>
struct ArrayRef; // external

/**
	@addtogroup lib_core_collection
	@{
*/

/**
	@addtogroup lib_core_array
	@{
*/

/// Dynamic array of POD objects.
template<class T>
struct Array {
	TOGO_CONSTRAIN_POD_COLLECTION(T);

	u32_fast _size;
	u32_fast _capacity;
	T* _data;
	Allocator* _allocator;

	Array() = delete;
	Array(Array<T> const&) = delete;
	Array& operator=(Array<T> const&) = delete;

	~Array();
	Array(Allocator& allocator);

	Array(Array<T>&&);
	Array& operator=(Array<T>&&);

	T& operator[](unsigned const i);
	T const& operator[](unsigned const i) const;

	operator ArrayRef<T>() const;
	operator ArrayRef<T const>() const;
};

/** @} */ // end of doc-group lib_core_array

/**
	@addtogroup lib_core_fixed_array
	@{
*/

/// Fixed-capacity array of POD objects.
template<class T, unsigned N>
struct FixedArray {
	TOGO_CONSTRAIN_POD_COLLECTION(T);

	enum : unsigned {
		/// Array capacity.
		CAPACITY = N
	};

	u32_fast _size;
	T _data[N];

	~FixedArray() = default;

	FixedArray(FixedArray<T, N> const&) = delete;
	FixedArray(FixedArray<T, N>&&) = delete;
	FixedArray& operator=(FixedArray<T, N> const&) = delete;
	FixedArray& operator=(FixedArray<T, N>&&) = delete;

	FixedArray();

	T& operator[](unsigned const i);
	T const& operator[](unsigned const i) const;
};

/** @} */ // end of doc-group lib_core_fixed_array

/**
	@addtogroup lib_core_queue
	@{
*/

/// Dynamic double-ended queue of POD objects.
template<class T>
struct Queue {
	TOGO_CONSTRAIN_POD_COLLECTION(T);

	Array<T> _data;
	u32_fast _head;
	u32_fast _size;

	~Queue() = default;

	Queue() = delete;
	Queue(Queue<T> const&) = delete;
	Queue& operator=(Queue<T> const&) = delete;

	Queue(Allocator& allocator);

	Queue(Queue<T>&&);
	Queue& operator=(Queue<T>&&);

	T& operator[](unsigned const i);
	T const& operator[](unsigned const i) const;
};

/** @} */ // end of doc-group lib_core_queue

/**
	@addtogroup lib_core_priority_queue
	@{
*/

/// Priority queue of POD objects.
template<class T>
struct PriorityQueue {
	TOGO_CONSTRAIN_POD_COLLECTION(T);

	/// Less-than comparison function type.
	using less_func_type = bool (
		T const& x,
		T const& y
	);

	Array<T> _data;
	less_func_type* _less_func;

	~PriorityQueue() = default;
	PriorityQueue(PriorityQueue<T>&&) = default;
	PriorityQueue& operator=(PriorityQueue<T>&&) = default;

	PriorityQueue() = delete;
	PriorityQueue(PriorityQueue<T> const&) = delete;
	PriorityQueue& operator=(PriorityQueue<T> const&) = delete;

	PriorityQueue(
		PriorityQueue<T>::less_func_type& less_func,
		Allocator& allocator
	);

	T& operator[](unsigned const i);
	T const& operator[](unsigned const i) const;
};

/** @} */ // end of doc-group lib_core_priority_queue

/**
	@addtogroup lib_core_hash_map
	@{
*/

/// HashMap node.
template<class K, class T>
struct HashMapNode {
	K key;
	u32 next;
	T value;
};

/// Hash map of POD objects.
template<class K, class T>
struct HashMap {
	TOGO_CONSTRAIN_POD_COLLECTION(T);
	#if defined(TOGO_USE_CONSTRAINTS)
		static_assert(
			is_same<K, hash32>::value ||
			is_same<K, hash64>::value,
			"key type must be hash32 or hash64"
		);
	#endif

	Array<u32> _head;
	Array<HashMapNode<K, T>> _data;

	~HashMap() = default;
	HashMap(HashMap<K, T>&&) = default;
	HashMap& operator=(HashMap<K, T>&&) = default;

	HashMap() = delete;
	HashMap(HashMap<K, T> const&) = delete;
	HashMap& operator=(HashMap<K, T> const&) = delete;

	HashMap(Allocator& allocator);
};

/** @} */ // end of doc-group lib_core_hash_map

/** @} */ // end of doc-group lib_core_collection

} // namespace togo
