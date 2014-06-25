/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file config.hpp
@brief Core configuration.
@ingroup config
*/

#pragma once

// AM‽ configuration
#include <am/config_values.hpp>

/** @cond INTERNAL */
#ifdef AM_CONFIG_IMPLICIT_LINEAR_INTERFACE
	#undef AM_CONFIG_IMPLICIT_LINEAR_INTERFACE
#endif
#define AM_CONFIG_FLOAT_PRECISION AM_PRECISION_MEDIUM
#define AM_CONFIG_VECTOR_TYPES AM_FLAG_TYPE_FLOAT
#define AM_CONFIG_MATRIX_TYPES AM_FLAG_TYPE_FLOAT
/** @endcond */ // INTERNAL

#include <am/config.hpp>

namespace togo {

/**
	@addtogroup config
	@{
*/

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/**
		Whether to use assertions

		This is enabled if it is defined.
	*/
	#define TOGO_DISABLE_ASSERTIONS
	/**
		Whether to use assertions

		This is enabled if it is defined. It is automatically enabled
		if DEBUG is defined or !NDEBUG.
	*/
	#define TOGO_DEBUG
	/**
		Whether to use stdlib constraints to ensure types are being
		used properly.

		This is enabled if it is defined.
	*/
	#define TOGO_USE_CONSTRAINTS
#else
	#if !defined(TOGO_DEBUG) && (defined(DEBUG) || !defined(NDEBUG))
		#define TOGO_DEBUG 1
	#endif
#endif

/** @} */ // end of doc-group config

} // namespace togo
