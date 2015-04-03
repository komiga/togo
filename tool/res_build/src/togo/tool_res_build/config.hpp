#line 2 "togo/tool_res_build/config.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Core configuration.
@ingroup tool_res_build_config

@defgroup tool_res_build_config Configuration
@ingroup tool_res_build
@details
*/

#pragma once

#include <togo/game/config.hpp>

namespace togo {
namespace tool_res_build {

/**
	@addtogroup tool_res_build_config
	@{
*/

/// tool_res_build information text.
#define TOGO_TOOL_RES_BUILD_INFO_TEXT \
	"togo res_build 0.00"

/// tool_res_build usage text.
#define TOGO_TOOL_RES_BUILD_USAGE_TEXT \
	"usage: build [options] <command> [command_arguments]"

/** @} */ // end of doc-group tool_res_build_config

} // namespace tool_res_build
} // namespace togo
