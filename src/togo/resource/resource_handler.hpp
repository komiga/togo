#line 2 "togo/resource/resource_handler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceHandler interface.
@ingroup resource
@ingroup resource_handler
*/

#pragma once

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/renderer/types.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource_manager.hpp>

namespace togo {

namespace resource_handler {

/**
	@addtogroup resource_handler
	@{
*/

/// Register test (TestResource) resource handler.
void register_test(
	ResourceManager& rm
);

/// Register shader_prelude (gfx::ShaderDef) resource handler.
void register_shader_prelude(
	ResourceManager& rm,
	gfx::Renderer* const renderer
);

/// Register shader (gfx::ShaderID) resource handler.
void register_shader(
	ResourceManager& rm,
	gfx::Renderer* const renderer
);

/// Register render_config (gfx::RenderConfig) resource handler.
void register_render_config(
	ResourceManager& rm,
	gfx::Renderer* const renderer
);

/** @} */ // end of doc-group resource_handler

} // namespace resource_handler

namespace resource {

/**
	@addtogroup resource
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

/// Load a render config.
///
/// An assertion will fail if the load failed.
inline gfx::RenderConfig* load_render_config(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	auto* render_config = static_cast<gfx::RenderConfig*>(
		resource_manager::load_resource(rm, RES_TYPE_RENDER_CONFIG, name_hash).pointer
	);
	TOGO_ASSERT(render_config, "failed to load render_config");
	return render_config;
}

/** @} */ // end of doc-group resource

} // namespace resource

} // namespace togo
