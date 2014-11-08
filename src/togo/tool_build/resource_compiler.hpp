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
	@addtogroup resource_compiler
	@{
*/

/// Register TestResource resource compiler.
void register_test_resource(
	CompilerManager& cm
);

/** @} */ // end of doc-group resource_compiler

} // namespace resource_compiler
} // namespace tool_build
} // namespace togo
