#line 2 "togo/window/config.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Window library configuration.
@ingroup lib_window_config

@defgroup lib_window_config Configuration
@ingroup lib_window
@details
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_window_config
	@{
*/

/** @name Window configuration */ /// @{

/// SDL window backend.
#define TOGO_WINDOW_BACKEND_SDL 1

/// GLFW window backend.
#define TOGO_WINDOW_BACKEND_GLFW 2

/// System-dependent window backend for raster graphics.
///
/// Uses the following by OS:
///
/// - Linux: XCB
#define TOGO_WINDOW_BACKEND_RASTER 3

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// Set the window backend.
	///
	/// Options:
	///
	/// - #TOGO_WINDOW_BACKEND_SDL (OpenGL)
	/// - #TOGO_WINDOW_BACKEND_GLFW (OpenGL)
	/// - #TOGO_WINDOW_BACKEND_RASTER
	#define TOGO_CONFIG_WINDOW_BACKEND

	/// Whether OpenGL is supported by the backend.
	#define TOGO_WINDOW_BACKEND_USES_OPENGL
#else
	#if !defined(TOGO_CONFIG_WINDOW_BACKEND)
		#error "window backend has not been selected"
	#endif

	#if (TOGO_CONFIG_WINDOW_BACKEND == TOGO_WINDOW_BACKEND_SDL) || \
		(TOGO_CONFIG_WINDOW_BACKEND == TOGO_WINDOW_BACKEND_GLFW)
		#define TOGO_WINDOW_BACKEND_USES_OPENGL
	#elif (TOGO_CONFIG_WINDOW_BACKEND != TOGO_WINDOW_BACKEND_RASTER)
		#error "TOGO_CONFIG_WINDOW_BACKEND has an invalid value"
	#endif
#endif // defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)

/// @} // end of name-group Window configuration

/** @} */ // end of doc-group lib_window_config

} // namespace togo
