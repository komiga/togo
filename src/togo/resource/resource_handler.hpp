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
#include <togo/resource/types.hpp>
#include <togo/resource/resource_manager.hpp>
#include <togo/gfx/types.hpp>

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

/** @} */ // end of doc-group resource_handler

} // namespace resource_handler
} // namespace togo
