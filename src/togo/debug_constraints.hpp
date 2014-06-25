/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file debug_constraints.hpp
@brief Definitions for constraints.
*/

#pragma once

#include <togo/config.hpp>

#if defined(TOGO_USE_CONSTRAINTS)
	#include <type_traits>

	#define TOGO_CONSTRAIN_IS_POD(T)								\
		static_assert(												\
			std::is_standard_layout<T>::value,						\
			"T is not POD"											\
		);
	#define TOGO_CONSTRAIN_SAME(T, U)								\
		static_assert(												\
			std::is_same<T, U>::value,								\
			"T and U are not the same types"						\
		);
#else
	/// Statically assert that T is of standard layout.
	#define TOGO_CONSTRAIN_IS_POD(T)

	/// Statically assert that type T is the same as type U.
	#define TOGO_CONSTRAIN_SAME(T, U)
#endif

namespace togo {
} // namespace togo
