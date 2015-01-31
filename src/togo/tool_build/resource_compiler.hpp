#line 2 "togo/tool_build/resource_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ResourceCompiler interface.
@ingroup tool_build_resource_compiler
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>

namespace togo {
namespace tool_build {
namespace resource_compiler {

/**
	@addtogroup tool_build_resource_compiler
	@{
*/

/// Register test resource compiler.
void register_test(
	CompilerManager& cm
);

/// Register shader_prelude resource compiler.
void register_shader_prelude(
	CompilerManager& cm
);

/// Register shader resource compiler.
void register_shader(
	CompilerManager& cm
);

/// Register render_config resource compiler.
void register_render_config(
	CompilerManager& cm,
	GfxCompiler& gfx_compiler
);

/// Register standard resource compilers.
void register_standard(
	CompilerManager& cm,
	GfxCompiler& gfx_compiler
);

/** @} */ // end of doc-group tool_build_resource_compiler

} // namespace resource_compiler
} // namespace tool_build
} // namespace togo
