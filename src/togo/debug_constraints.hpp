#line 2 "togo/debug_constraints.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Definitions for constraints.
@ingroup debug
*/

#pragma once

#include <togo/config.hpp>

/**
	@addtogroup debug
	@{
*/

#if defined(TOGO_USE_CONSTRAINTS)
	#include <togo/traits.hpp>
	#include <type_traits>

	#define TOGO_CONSTRAIN_POD(T)									\
		static_assert(												\
			std::is_standard_layout<T>::value,						\
			"T is not a POD type"									\
		)

	#define TOGO_CONSTRAIN_SAME(T, U)								\
		static_assert(												\
			is_same<T, U>::value,									\
			"T and U are not the same types"						\
		)

	#define TOGO_CONSTRAIN_INTEGRAL(T)								\
		static_assert(												\
			is_integral<T>::value,									\
			"T is not an integral type"								\
		)

	#define TOGO_CONSTRAIN_ARITHMETIC(T)							\
		static_assert(												\
			is_arithmetic<T>::value,								\
			"T is not an arithmetic type"							\
		)

	#define TOGO_CONSTRAIN_FLOATING_POINT(T)						\
		static_assert(												\
			is_floating_point<T>::value,							\
			"T is not a floating-point type"						\
		)

	#define TOGO_CONSTRAIN_UNSIGNED(T)								\
		static_assert(												\
			is_unsigned<T>::value,									\
			"T is not an unsigned arithmetic type"					\
		)
#else
	/// Statically assert that type T is of standard layout.
	#define TOGO_CONSTRAIN_POD(T)

	/// Statically assert that type T is the same as type U.
	#define TOGO_CONSTRAIN_SAME(T, U)

	/// Statically assert that type T is an integral type.
	#define TOGO_CONSTRAIN_INTEGRAL(T)

	/// Statically assert that type T is an arithmetic type.
	#define TOGO_CONSTRAIN_ARITHMETIC(T)

	/// Statically assert that type T is a floating-point type.
	#define TOGO_CONSTRAIN_FLOATING_POINT(T)

	/// Statically assert that type T is an unsigned integral type.
	#define TOGO_CONSTRAIN_UNSIGNED(T)
#endif

/** @} */ // end of doc-group debug
