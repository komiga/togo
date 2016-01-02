#line 2 "togo/core/lua/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Scripting types.
@ingroup lib_core_types
@ingroup lib_core_lua
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/string/types.hpp>

namespace togo {

/// Lua module source reference.
struct LuaModuleRef {
	StringRef name;
	StringRef source;
};

} // namespace togo
