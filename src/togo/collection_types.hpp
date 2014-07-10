#line 2 "togo/collection_types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file collection_types.hpp
@brief Collection types.
@ingroup types
@ingroup collections
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/memory_types.hpp>

#include <togo/debug_constraints.hpp>

namespace togo {

/**
	@addtogroup collections
	@{
*/

/**
	@addtogroup array
	@{
*/

/**
	Dynamic array of POD objects.
*/
template<class T>
struct Array {
	TOGO_CONSTRAIN_POD(T);

	u32 _size;
	u32 _capacity;
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
};

/** @} */ // end of doc-group array

/**
	@addtogroup queue
	@{
*/

/**
	Dynamic double-ended queue of POD objects.
*/
template<class T>
struct Queue {
	TOGO_CONSTRAIN_POD(T);

	Array<T> _data;
	u32 _head;
	u32 _size;

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

/** @} */ // end of doc-group queue

/**
	@addtogroup priority_queue
	@{
*/

/**
	Priority queue of POD objects.
*/
template<class T>
struct PriorityQueue {
	TOGO_CONSTRAIN_POD(T);

	using less_func_type
	= bool (&)(T const&, T const&);

	Array<T> _data;
	less_func_type _less_func;

	~PriorityQueue() = default;
	PriorityQueue(PriorityQueue<T>&&) = default;
	PriorityQueue& operator=(PriorityQueue<T>&&) = default;

	PriorityQueue() = delete;
	PriorityQueue(PriorityQueue<T> const&) = delete;
	PriorityQueue& operator=(PriorityQueue<T> const&) = delete;

	PriorityQueue(
		PriorityQueue<T>::less_func_type less_func,
		Allocator& allocator
	);

	T& operator[](unsigned const i);
	T const& operator[](unsigned const i) const;
};

/** @} */ // end of doc-group priority_queue

/** @} */ // end of doc-group collections

} // namespace togo
