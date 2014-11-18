#line 2 "togo/hash/hash_combiner.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief HashCombiner interface.
@ingroup hash
@ingroup hash_combiner
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/string/string.hpp>
#include <togo/hash/types.hpp>
#include <togo/hash/hash.hpp>

namespace togo {
namespace hash_combiner {

/**
	@addtogroup hash_combiner
	@{
*/

/// Number of bytes combined.
template<class H>
inline unsigned size(HashCombiner<H> const& combiner) {
	return combiner._impl.size();
}

/// Returns true if any bytes have been combined.
template<class H>
inline bool any(HashCombiner<H> const& combiner) {
	return hash_combiner::size(combiner) != 0;
}

/// Returns true if no bytes have been combined.
template<class H>
inline bool empty(HashCombiner<H> const& combiner) {
	return hash_combiner::size(combiner) == 0;
}

/// Initialize.
///
/// This will return the combiner to its initial state.
/// The combiner's initial state is initialized, so this does not
/// need to be called right after constructing the object.
template<class H>
inline void init(
	HashCombiner<H>& combiner
) {
	combiner._impl.init();
}

/// Add bytes.
template<class H>
inline void add(
	HashCombiner<H>& combiner,
	char const* const data,
	unsigned const size
) {
	combiner._impl.add(data, size);
}

/// Add string.
template<class H>
inline void add(
	HashCombiner<H>& combiner,
	StringRef const& str
) {
	hash_combiner::add(combiner, str.data, str.size);
}

/// Calculate current value.
///
/// This does not affect the state of the combiner.
template<class H>
inline H value(HashCombiner<H> const& combiner) {
	return
		hash_combiner::empty(combiner)
		? hash::traits<H>::identity
		: combiner._impl.value()
	;
}

/** @} */ // end of doc-group hash_combiner

} // namespace hash_combiner
} // namespace togo
