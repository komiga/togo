#line 2 "togo/tool_res_build/interface.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief tool_res_build interface.
@ingroup tool_res_build_interface

@defgroup tool_res_build_interface Interface
@ingroup tool_res_build
@details
*/

#pragma once

// igen-following-sources-included
// igen-source-pattern: interface/.+ipp

#include <togo/core/string/types.hpp>
#include <togo/core/kvs/types.hpp>
#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/tool_res_build/interface.gen_interface>

namespace togo {
namespace tool_res_build {
namespace interface {

/**
	@addtogroup tool_res_build_interface
	@{
*/

/// Project path.
inline StringRef project_path(
	Interface const& interface
) {
	return {interface._project_path};
}

/** @} */ // end of doc-group tool_res_build_interface

} // namespace interface
} // namespace tool_res_build
} // namespace togo
