#line 2 "togo/game/tool_build/interface.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief tool_build interface.
@ingroup tool_build_interface
*/

#pragma once

// igen-following-sources-included
// igen-source-pattern: tool_build/interface/.+ipp

#include <togo/game/tool_build/config.hpp>
#include <togo/game/tool_build/types.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/kvs/types.hpp>
#include <togo/game/tool_build/interface.gen_interface>

namespace togo {
namespace tool_build {
namespace interface {

/**
	@addtogroup tool_build_interface
	@{
*/

/// Project path.
inline StringRef project_path(
	Interface const& interface
) {
	return {interface._project_path};
}

/** @} */ // end of doc-group tool_build_interface

} // namespace interface
} // namespace tool_build
} // namespace togo
