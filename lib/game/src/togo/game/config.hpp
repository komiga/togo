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
#include <togo/window/config.hpp>

namespace togo {
namespace game {

/**
	@addtogroup lib_game_config
	@{
*/

/** @name Graphics configuration */ /// @{

/// OpenGL renderer.
#define TOGO_RENDERER_OPENGL 0x00000001

#if defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)
	/// Configure the renderer to use.
	///
	/// Options:
	///
	/// - #TOGO_RENDERER_OPENGL (default)
	#define TOGO_CONFIG_RENDERER
#else
	#if !defined(TOGO_CONFIG_RENDERER)
		#define TOGO_CONFIG_RENDERER TOGO_RENDERER_OPENGL
	#endif

	#if (TOGO_CONFIG_RENDERER != TOGO_RENDERER_OPENGL)
		#error "TOGO_CONFIG_RENDERER has an invalid value"
	#endif
#endif // defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI)

/// @} // end of name-group Graphics configuration

/** @} */ // end of doc-group lib_game_config

} // namespace game
} // namespace togo
