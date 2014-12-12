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

/** @} */ // end of doc-group tool_build_resource_compiler

} // namespace resource_compiler
} // namespace tool_build
} // namespace togo
