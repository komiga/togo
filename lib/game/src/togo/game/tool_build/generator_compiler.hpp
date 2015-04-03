#line 2 "togo/game/tool_build/generator_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief GeneratorCompiler interface.
@ingroup tool_build_gfx_compiler
*/

#pragma once

#include <togo/game/tool_build/config.hpp>
#include <togo/game/tool_build/types.hpp>
#include <togo/game/tool_build/generator_compiler.gen_interface>

namespace togo {
namespace tool_build {
namespace generator_compiler {

/**
	@addtogroup tool_build_gfx_compiler
	@{
*/

/// test_proxy generator compiler.
extern GeneratorCompiler const test_proxy;

/// clear generator compiler.
extern GeneratorCompiler const clear;

/** @} */ // end of doc-group tool_build_gfx_compiler

} // namespace generator_compiler
} // namespace tool_build
} // namespace togo
