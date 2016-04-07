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

/// Load or reference a shader.
inline gfx::ShaderID ref_shader(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	auto* resource = resource_manager::ref_load(rm, RES_TYPE_SHADER, name_hash);
	TOGO_ASSERT(resource, "failed to load shader");
	return gfx::ShaderID{resource->value.uinteger};
}

/// Unreference a shader.
inline unsigned unref_shader(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	return resource_manager::unref(rm, RES_TYPE_SHADER, name_hash);
}

/// Load or reference a render configuration.
inline gfx::PackedRenderConfig* ref_render_config(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	auto* resource = resource_manager::ref_load(rm, RES_TYPE_RENDER_CONFIG, name_hash);
	TOGO_ASSERT(resource, "failed to load render_config");
	return static_cast<gfx::PackedRenderConfig*>(resource->value.pointer);
}

/// Unreference a render configuration.
inline unsigned unref_render_config(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	return resource_manager::unref(rm, RES_TYPE_RENDER_CONFIG, name_hash);
}

/** @} */ // end of doc-group lib_game_resource

} // namespace resource
} // namespace game
} // namespace togo
