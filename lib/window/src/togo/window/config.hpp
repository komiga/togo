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

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// Set the window backend.
	///
	/// Options:
	///
	/// - #TOGO_WINDOW_BACKEND_SDL (OpenGL, raster)
	/// - #TOGO_WINDOW_BACKEND_GLFW (OpenGL)
	#define TOGO_CONFIG_WINDOW_BACKEND

	/// Defined if OpenGL windows are supported by the backend.
	#define TOGO_WINDOW_BACKEND_SUPPORTS_OPENGL

	/// Defined if raster windows are supported by the backend.
	#define TOGO_WINDOW_BACKEND_SUPPORTS_RASTER
#else
	#if !defined(TOGO_CONFIG_WINDOW_BACKEND)
		#error "window backend has not been selected"
	#endif

	#if (TOGO_CONFIG_WINDOW_BACKEND == TOGO_WINDOW_BACKEND_SDL) || \
		(TOGO_CONFIG_WINDOW_BACKEND == TOGO_WINDOW_BACKEND_GLFW)
		#define TOGO_WINDOW_BACKEND_SUPPORTS_OPENGL
	#endif
	#if (TOGO_CONFIG_WINDOW_BACKEND == TOGO_WINDOW_BACKEND_SDL)
		#define TOGO_WINDOW_BACKEND_SUPPORTS_RASTER
	#endif
#endif // defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)

/// @} // end of name-group Window configuration

/** @} */ // end of doc-group lib_window_config

} // namespace togo
