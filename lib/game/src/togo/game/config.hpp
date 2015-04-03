#line 2 "togo/game/config.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Game library configuration.
@ingroup lib_game_config

@defgroup lib_game_config Configuration
@ingroup lib_game
@details
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_game_config
	@{
*/

/** @name Graphics configuration */ /// @{

/// OpenGL renderer.
#define TOGO_RENDERER_OPENGL 0x00000100

/// GLFW graphics backend.
///
/// This backend can be used with the following renderers:
///
/// - #TOGO_RENDERER_OPENGL
#define TOGO_GRAPHICS_BACKEND_GLFW 0x00000001

/// SDL graphics backend.
///
/// This backend can be used with the following renderers:
///
/// - #TOGO_RENDERER_OPENGL
#define TOGO_GRAPHICS_BACKEND_SDL 0x00000002

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// Configure the graphics backend to use.
	///
	/// Options:
	///
	/// - #TOGO_GRAPHICS_BACKEND_GLFW
	/// - #TOGO_GRAPHICS_BACKEND_SDL
	#define TOGO_CONFIG_GRAPHICS_BACKEND

	/// Configure the renderer to use.
	///
	/// Options:
	///
	/// - #TOGO_RENDERER_OPENGL
	#define TOGO_CONFIG_RENDERER
#else
	#if !defined(TOGO_CONFIG_RENDERER)
		#error "renderer has not been selected"
	#endif
	#if !defined(TOGO_CONFIG_GRAPHICS_BACKEND)
		#error "graphics backend has not been selected"
	#endif

	#if (TOGO_CONFIG_RENDERER != TOGO_RENDERER_OPENGL)
		#error "TOGO_CONFIG_RENDERER has an invalid value"
	#endif
	#if (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_GLFW)
		#if (TOGO_CONFIG_RENDERER != TOGO_RENDERER_OPENGL)
			#error "selected renderer not usable with GLFW graphics backend"
		#endif
	#elif (TOGO_CONFIG_GRAPHICS_BACKEND == TOGO_GRAPHICS_BACKEND_SDL)
		#if (TOGO_CONFIG_RENDERER != TOGO_RENDERER_OPENGL)
			#error "selected renderer not usable with SDL graphics backend"
		#endif
	#else
		#error "TOGO_CONFIG_GRAPHICS_BACKEND has an invalid value"
	#endif
#endif // defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)

/// @} // end of name-group Graphics configuration

/** @} */ // end of doc-group lib_game_config

} // namespace togo
