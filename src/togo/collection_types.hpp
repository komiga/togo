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
	TOGO_CONSTRAIN_IS_POD(T)

	u32 _size;
	u32 _capacity;
	T* _data;
	Allocator* _allocator;

	Array(Array<T>&&) = default;
	Array& operator=(Array<T>&&) = default;

	Array() = delete;
	Array(Array<T> const&) = delete;
	Array& operator=(Array<T> const&) = delete;

	~Array();
	Array(Allocator& allocator);

	T& operator[](u32 const i);
	T const& operator[](u32 const i) const;
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
	TOGO_CONSTRAIN_IS_POD(T)

	Array<T> _data;
	u32 _head;
	u32 _size;

	~Queue() = default;
	Queue(Queue<T>&&) = default;
	Queue& operator=(Queue<T>&&) = default;

	Queue() = delete;
	Queue(Queue<T> const&) = delete;
	Queue& operator=(Queue<T> const&) = delete;

	Queue(Allocator& allocator);

	T& operator[](u32 const i);
	T const& operator[](u32 const i) const;
};

/** @} */ // end of doc-group queue

/** @} */ // end of doc-group collections

} // namespace togo
