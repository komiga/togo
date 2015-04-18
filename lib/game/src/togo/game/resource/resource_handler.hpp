#line 2 "togo/game/resource/resource_handler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceHandler interface.
@ingroup lib_game_resource
@ingroup lib_game_resource_handler

@defgroup lib_game_resource_handler ResourceHandler
@ingroup lib_game_resource
@details
*/

#pragma once

// igen-source-pattern: resource/resource_handler/.+cpp

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/renderer/types.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource_manager.hpp>
#include <togo/game/resource/resource_handler.gen_interface>

namespace togo {
namespace game {
namespace resource {

/**
	@addtogroup lib_game_resource
	@{
*/

/// Load a shader.
///
/// An assertion will fail if the shader failed to load.
inline gfx::ShaderID load_shader(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	gfx::ShaderID const id{
		resource_manager::load_resource(rm, RES_TYPE_SHADER, name_hash).uinteger
	};
	TOGO_ASSERT(id.valid(), "failed to load shader");
	return id;
}

/// Load a render configuration.
///
/// An assertion will fail if the load failed.
inline gfx::PackedRenderConfig* load_render_config(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	auto* packed_config = static_cast<gfx::PackedRenderConfig*>(
		resource_manager::load_resource(rm, RES_TYPE_RENDER_CONFIG, name_hash).pointer
	);
	TOGO_ASSERT(packed_config, "failed to load render_config");
	return packed_config;
}

/** @} */ // end of doc-group lib_game_resource

} // namespace resource
} // namespace game
} // namespace togo
