#line 2 "togo/core/math/matrix.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Matrix interface.
@ingroup lib_core_math
@ingroup lib_core_math_matrix
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/math/traits.hpp>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math_matrix
	@{
*/

/// Transpose a matrix.
template<class C>
inline typename C::transpose_type transpose(C const& m) {
	static_assert(math::is_matrix<C>::value, "");
	return C::operations::transpose(m);
}

/// Calculate the determinant of a (square) matrix.
template<class C>
inline math::value_type<C> determinant(C const& m) {
	static_assert(math::is_square_matrix<C>::value, "");
	return C::operations::determinant(m);
}

/// Calculate the inverse of a (square) matrix.
///
/// @warning The values in the resultant matrix are undefined if m is singular.
template<class C>
inline C inverse(C const& m) {
	static_assert(math::is_square_matrix<C>::value, "");
	return C::operations::inverse(m);
}

/** @} */ // end of doc-group lib_core_math_matrix

} // namespace math
} // namespace togo
