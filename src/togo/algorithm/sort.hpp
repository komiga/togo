#line 2 "togo/algorithm/sort.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Algorithms.
@ingroup algorithm
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/utility/constraints.hpp>

#include <cstring>

#undef TOGO_TEST_LOG_ENABLE
#if defined(TOGO_TEST_ALGORITHM)
	#define TOGO_TEST_LOG_ENABLE 
#endif
#include <togo/log/test.hpp>

namespace togo {

/**
	@addtogroup algorithm
	@{
*/

/// 2048-radix sort a T-value K-keyed collection with B-value bins.
///
/// T must be POD. K and B must be unsigned integral types.
///
/// KFunc is a key accessor function type or function object type.
/// key_func should return K for a given T.
///
/// The maximum value of B determines the maximum size of the input
/// collection. B = u32 performs the best in most cases and will
/// accept any sane collection size.
///
/// This makes a pass over the input collection for each
/// 11-bit slice of the key, starting at the LSB.
/// For example (note doubling):
///
/// - an 8-bit key makes 1 pass
/// - a 16-bit key makes 2 passes
/// - a 32-bit key makes 3 passes
/// - a 64-bit key makes 6 passes
///
/// values and values_swap are swapped for each pass. The return
/// value is true if they have been swapped at exit (i.e., values_swap
/// would contain the sorted result).
///
/// This is a stable sort that costs a swap buffer in memory and 2048
/// bins on the stack (1 bin for each value in the 11-bit key slice).
/// Its worst-case (and probably average) time complexity is linear:
/// O(P(2N + 3 * 2048)). However, due to tight loops and inner count
/// sorting, this should (if configured optimally) perform better than
/// a stable mergesort.
///
/// Due to the doubling of passes as the size of the key type doubles,
/// the time taken sorting N items with a key of K bits is roughly
/// half that of sorting the same N items with 2K bits.
///
/// Due to N assignments for each pass over the entire input in the
/// typical case, this works best with small T.
template<class T, class K, class B, class KFunc>
bool sort_radix_generic(
	T*& values,
	T*& values_swap,
	unsigned const size,
	KFunc key_func
) noexcept {
	TOGO_CONSTRAIN_POD(T);
	TOGO_CONSTRAIN_UNSIGNED(K);
	TOGO_CONSTRAIN_UNSIGNED(B);

	enum : unsigned {
		SLICE_BITS = 11,
		NUM_BINS = 1 << SLICE_BITS,
		SLICE_MASK = NUM_BINS - 1,
		NUM_PASSES
			= (sizeof(K) << 3) / SLICE_BITS
			+ !!((sizeof(K) << 3) % SLICE_BITS)
		,
	};

	/*TOGO_LOG_DEBUGF(
		"SLICE_BITS = %u  SLICE_MASK = %x  "
		"NUM_BINS = %u  NUM_PASSES = %u\n",
		SLICE_BITS, SLICE_MASK,
		NUM_BINS, NUM_PASSES
	);*/

	TOGO_ASSERT(
		size <= static_cast<B>(-1),
		"size exceeds limit for bin value type"
	);

	B bins[NUM_BINS]{0};
	K key;
	K pkey;
	B num_slices;
	B offset;
	unsigned pass = 0;
	unsigned i;
	unsigned slice_shift = 0;
	unsigned sorted;
	while (pass < NUM_PASSES) {
		// Count values for this slice
		sorted = 1;
		pkey = 0;
		for (i = 0; i < size; ++i) {
			key = key_func(values[i]);
			++bins[(key >> slice_shift) & SLICE_MASK];
			sorted &= pkey <= key;
			pkey = key;
		}

		// Quick exit if the collection is already sorted
		if (sorted) {
			break;
		}

		// Make each bin value an offset to the first item position
		// of each slice
		offset = 0;
		for (i = 0; i < NUM_BINS; ++i) {
			num_slices = bins[i];
			bins[i] = offset;
			offset += num_slices;
		}

		// Assign items according to offsets
		for (i = 0; i < size; ++i) {
			T const& value = values[i];
			offset = bins[(key_func(value) >> slice_shift) & SLICE_MASK]++;
			values_swap[offset] = value;
		}

		slice_shift += SLICE_BITS;
		swap(values, values_swap);
		if (++pass < NUM_PASSES) {
			std::memset(bins, 0, sizeof(bins));
		}
	}
	// Either quick exit hit or NUM_PASSES is an odd number
	return (pass & 1);
}

/// 2048-radix sort a 32-bit keyed collection with 32-bit bin values.
template<class T, class KFunc>
void sort_radix_k32(
	T* values,
	T* values_swap,
	unsigned const size,
	KFunc&& key_func
) noexcept {
	bool const swapped = sort_radix_generic<T, u32, u32>(
		values, values_swap, size, key_func
	);
	if (swapped) {
		// values from generic always points to the actual data;
		// ergo: with a swap, values_swap is the original input values
		std::memcpy(values_swap, values, size * sizeof(T));
	}
}

/// 2048-radix sort a 64-bit keyed collection with 32-bit bin values.
template<class T, class KFunc>
void sort_radix_k64(
	T* values,
	T* values_swap,
	unsigned const size,
	KFunc&& key_func
) noexcept {
	bool const swapped = sort_radix_generic<T, u64, u32>(
		values, values_swap, size, key_func
	);
	if (swapped) {
		std::memcpy(values_swap, values, size * sizeof(T));
	}
}

/// Insertion sort a collection.
///
/// T must be POD.
///
/// LFunc must be a 2-arity comparison function returning true
/// if arg1 < arg2 (less-than).
///
/// This implementation is in-place and stable.
///
/// Insertion sort is fast only for extremely small collections.
/// If the collection is near or above 200 values, another sort
/// should be considered instead.
template<class T, class LFunc>
void sort_insertion(
	T* const begin,
	T* const end,
	LFunc less_func
) {
	TOGO_CONSTRAIN_POD(T);

	T ivalue;
	T* i;
	T* j;
	for (i = begin + 1; i < end; ++i) {
		// Take value at current position
		ivalue = *i;

		// Shift elements behind ivalue down until ivalue would
		// be in its sub-sorted position
		for (j = i; j > begin && less_func(ivalue, *(j - 1)); --j) {
			*j = *(j - 1);
		}

		// Place ivalue in its position
		if (j != i) {
			*j = ivalue;
		}
	}
}

/** @} */ // end of doc-group algorithm

} // namespace togo

#include <togo/log/test_unconfigure.hpp>
