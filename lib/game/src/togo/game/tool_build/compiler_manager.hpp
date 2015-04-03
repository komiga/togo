#line 2 "togo/game/tool_build/compiler_manager.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief CompilerManager interface.
@ingroup tool_build_compiler_manager

@defgroup tool_build_compiler_manager CompilerManager
@ingroup tool_build
@details
*/

#pragma once

#include <togo/game/tool_build/config.hpp>
#include <togo/game/tool_build/types.hpp>
#include <togo/core/string/types.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/tool_build/compiler_manager.gen_interface>

namespace togo {
namespace tool_build {
namespace compiler_manager {

/**
	@addtogroup tool_build_compiler_manager
	@{
*/

/// Package collection.
inline Array<PackageCompiler*> const& packages(
	CompilerManager const& cm
) {
	return cm._packages;
}

/** @} */ // end of doc-group tool_build_compiler_manager

} // namespace compiler_manager
} // namespace tool_build
} // namespace togo
