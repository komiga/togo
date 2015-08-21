#line 2 "togo/core/collection/npod.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Non-POD interface.
@ingroup lib_core_collection
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/utility.hpp>

#include <cstring>

namespace togo {

namespace {

template<class T, bool const = enable_collection_construction_and_destruction<T>::value>
struct collection_npod_impl {
	inline static void construct(T*, T*) {}
	inline static void destruct(T*, T*) {}

	inline static void copy(
		T* dst, unsigned /*dst_size*/,
		T* src, unsigned src_size
	) {
		std::memcpy(dst, src, src_size * sizeof(T));
	}

	inline static void remove_over(T* data, unsigned new_size, unsigned i) {
		data[i] = data[new_size];
	}
};

template<class T>
struct collection_npod_impl<T, true> {
	inline static void construct(T* begin, T* end) {
		for (; begin < end; ++begin) {
			new(begin) T();
		}
	}

	inline static void destruct(T* begin, T* end) {
		for (; begin < end; ++begin) {
			begin->~T();
		}
	}

	inline static void copy(
		T* dst, unsigned dst_size,
		T* src, unsigned src_size
	) {
		if (dst_size > src_size) {
			destruct(dst + src_size, dst + dst_size);
			dst_size = src_size;
		}
		T* end = dst + dst_size;
		for (; dst < end; ++dst, ++src) {
			(*dst) = *src;
		}
		end += src_size - dst_size;
		for (; dst < end; ++dst, ++src) {
			new (dst) T(*src);
		}
	}

	inline static void remove_over(T* data, unsigned new_size, unsigned i) {
		destruct(data + i, data + i + 1);
		if (i < new_size) {
			std::memcpy(data + i, data + new_size, sizeof(T));
		}
	}
};

} // anonymous namespace

template<class T>
inline void collection_npod_construct(T* a, T* b) {
	collection_npod_impl<T>::construct(a, b);
}

template<class T>
inline void collection_npod_destruct(T* a, T* b) {
	collection_npod_impl<T>::destruct(a, b);
}

template<class T>
inline void collection_npod_copy(
	T* dst, unsigned dst_size,
	T* src, unsigned src_size
) {
	collection_npod_impl<T>::copy(dst, dst_size, src, src_size);
}

template<class T>
inline void collection_npod_remove_over(T* data, unsigned new_size, unsigned i) {
	collection_npod_impl<T>::remove_over(data, new_size, i);
}

} // namespace togo
