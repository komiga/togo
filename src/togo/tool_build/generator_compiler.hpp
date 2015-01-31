#line 2 "togo/tool_build/generator_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief GeneratorCompiler interface.
@ingroup tool_build_gfx_compiler
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>

namespace togo {
namespace tool_build {
namespace generator_compiler {

/**
	@addtogroup tool_build_gfx_compiler
	@{
*/

/// test_proxy generator compiler.
extern GeneratorCompiler const test_proxy;

/// Register standard generator compilers.
void register_standard(
	GfxCompiler& gfx_compiler
);

/** @} */ // end of doc-group tool_build_gfx_compiler

} // namespace generator_compiler
} // namespace tool_build
} // namespace togo
