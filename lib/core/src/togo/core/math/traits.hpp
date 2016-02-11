#line 2 "togo/core/math/traits.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Type traits.
@ingroup lib_core_math
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {
namespace math {

/**
	@addtogroup lib_core_math
	@{
*/

/**
	Whether the given type is a vector.
*/
template<class T>
struct is_vector : false_type {};

/**
	Whether the given type is a matrix.
*/
template<class T>
struct is_matrix : false_type {};

/**
	Whether the given type is a square matrix.
*/
template<class T>
struct is_square_matrix : false_type {};

namespace {

template<class C, class = void>
struct value_type_impl;

template<class C>
struct value_type_impl< C, enable_if<is_vector<C>::value || is_matrix<C>::value>> {
	using type = typename C::value_type;
};

template<class C>
struct value_type_impl<C, enable_if<is_arithmetic<C>::value>> {
	using type = C;
};

} // anonymous namespace

/**
	Value type of a linear construct or scalar.

	@tparam C An arithmetic type or specialized linear construct.
*/
template<class C>
using value_type = typename value_type_impl<C>::type;

/**
	Whether the components in a linear construct are floating-point
	arithmetic types.

	@warning This assumes C is a linear construct such as a matrix or a vector
	(i.e., a type that has a @c value_type member type alias).
*/
template<class C>
struct is_floating_point {
	/** Whether the linear construct is floating-point. */
	static constexpr bool value = is_floating_point<typename C::value_type>::value;
};

/** @cond INTERNAL */
#define TOGO_DECLARE_TYPE_IS_VECTOR(TYPE)							\
	template<class T>												\
	struct is_vector<TYPE<T>> : true_type							\
	{} /**/

#define TOGO_DECLARE_TYPE_IS_MATRIX(TYPE)							\
	template<class T>												\
	struct is_matrix<TYPE<T>> : true_type							\
	{} /**/

#define TOGO_DECLARE_TYPE_IS_SQUARE_MATRIX(TYPE)					\
	template<class T>												\
	struct is_square_matrix<TYPE<T>> : true_type					\
	{} /**/
/** @endcond */

/** @} */ // end of doc-group lib_core_math

} // namespace math
} // namespace togo
