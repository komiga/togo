#line 2 "togo/tool_build/gfx_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief GfxCompiler interface.
@ingroup tool_build_gfx_compiler
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>

namespace togo {
namespace tool_build {
namespace gfx_compiler {

/**
	@addtogroup tool_build_gfx_compiler
	@{
*/

/// Register a generator compiler.
void register_generator_compiler(
	GfxCompiler& gfx_compiler,
	GeneratorCompiler const& gen_compiler
);

/// Find a generator compiler by name.
GeneratorCompiler* find_generator_compiler(
	GfxCompiler& gc,
	gfx::GeneratorNameHash name_hash
);

/** @} */ // end of doc-group tool_build_gfx_compiler

} // namespace gfx_compiler
} // namespace tool_build
} // namespace togo
