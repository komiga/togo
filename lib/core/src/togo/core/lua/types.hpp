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
#include <togo/core/types.hpp>
#include <togo/core/utility/tags.hpp>
#include <togo/core/string/types.hpp>

extern "C" {
	#include <luajit-2.0/luaconf.h>

	// Forward declarations
	struct lua_State;
}

namespace togo {

/// Get the name of a Lua interface function.
#define TOGO_LI_FUNC(name) li_##name

/// Define a Lua interface function.
#define TOGO_LI_FUNC_DEF(name) static signed TOGO_LI_FUNC(name)(lua_State* L)

/// Reference a Lua interface function for a library function table.
#define TOGO_LI_FUNC_REF(interface, name) {#name, interface :: TOGO_LI_FUNC(name)},

/// Mark a class as valid userdata.
///
/// Use this within the class definition.
#define TOGO_LUA_MARK_USERDATA(class_name) \
	static constexpr char const lua_metatable_name[]{#class_name}

/// Anchor a previous marker to shut up the linker.
///
/// Use this within the interface implementation file.
#define TOGO_LUA_MARK_USERDATA_ANCHOR(class_name) \
	constexpr char const class_name::lua_metatable_name[];

// FIXME: Do this with a type switch? If using standard Lua, this can be
// a different size.

/// Lua integral type.
using LuaInt = s64;
static_assert(sizeof(s64) == sizeof(LUA_INTEGER), "expected 64-bit LUA_INTEGER");

/// Lua floating-point type.
using LuaFloat = LUA_NUMBER;

/// Lua C function.
using LuaCFunction = signed(lua_State* L);

/// Lua module source reference.
struct LuaModuleRef {
	StringRef name;
	StringRef source;
};

} // namespace togo
