#line 2 "togo/tool_res_build/generator_compiler.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief GeneratorCompiler interface.
@ingroup tool_res_build_generator_compiler

@defgroup tool_res_build_generator_compiler GeneratorCompiler
@ingroup tool_res_build
@details
*/

#pragma once

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/tool_res_build/generator_compiler.gen_interface>

namespace togo {
namespace tool_res_build {
namespace generator_compiler {

/**
	@addtogroup tool_res_build_generator_compiler
	@{
*/

/// test_proxy generator compiler.
extern GeneratorCompiler const test_proxy;

/// clear generator compiler.
extern GeneratorCompiler const clear;

/** @} */ // end of doc-group tool_res_build_generator_compiler

} // namespace generator_compiler
} // namespace tool_res_build
} // namespace togo
