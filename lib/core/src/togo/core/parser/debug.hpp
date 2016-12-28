#line 2 "togo/core/parser/debug.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Parser debug variables.
@ingroup lib_core_parser
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>

namespace togo {
namespace parser {

#if defined(TOGO_DEBUG)
extern thread_local unsigned s_debug_trace_depth;
extern thread_local unsigned s_debug_error_gen;
extern thread_local unsigned s_debug_error_last;
extern thread_local bool s_debug_error_show;
#endif

} // namespace parser
} // namespace togo
