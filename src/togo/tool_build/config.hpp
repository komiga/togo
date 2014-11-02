#line 2 "togo/tool_build/config.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Core configuration.
@ingroup tool_build_config
*/

#pragma once

#include <togo/config.hpp>

namespace togo {
namespace tool_build {

/**
	@addtogroup tool_build_config
	@{
*/

/// tool_build information text.
#define TOGO_TOOL_BUILD_INFO_TEXT \
	"togo tool_build 0.00"

/// tool_build usage text.
#define TOGO_TOOL_BUILD_USAGE_TEXT \
	"usage: build [options] <command> [command_arguments]"

/** @} */ // end of doc-group tool_build_config

} // namespace tool_build
} // namespace togo
