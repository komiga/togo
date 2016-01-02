#line 2 "togo/core/lua/lua.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Scripting interface.
@ingroup lib_core_lua
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/lua/types.hpp>

extern "C" {
	#include <luajit-2.0/lua.h>
	#include <luajit-2.0/lauxlib.h>
	#include <luajit-2.0/lualib.h>
}

#include <togo/core/lua/lua.gen_interface>

namespace togo {
namespace lua {

/**
	@addtogroup lib_core_lua
	@{
*/

/// Get an argument from the stack as light userdata.
inline void* get_lightuserdata(lua_State* L, signed narg, bool require = true) {
	luaL_checktype(L, narg, LUA_TLIGHTUSERDATA);
	auto p = lua_touserdata(L, narg);
	if (require && !p) {
		luaL_argerror(L, narg, "value must be non-null");
	}
	return p;
}

/// Get an argument from the stack (typed).
///
/// This is an unchecked cast.
template<class T>
inline T* get_lightuserdata_typed(lua_State* L, signed narg, bool require = true) {
	return static_cast<T*>(get_lightuserdata(L, narg, require));
}

/// Get an argument from the stack as a boolean.
inline bool get_boolean(lua_State* L, signed narg) {
	luaL_checktype(L, narg, LUA_TBOOLEAN);
	return lua_toboolean(L, narg);
}

/// Get a string from the stack as a string reference.
inline StringRef get_string(lua_State* L, signed narg) {
	size_t size = 0;
	auto data = luaL_checklstring(L, narg, &size);
	return {data, static_cast<unsigned>(size)};
}

/// Push a string to the stack.
inline void push_string(lua_State* L, StringRef str) {
	lua_pushlstring(L, str.data ? str.data : "", unsigned_cast(str.size));
}

/// Push a light userdata to the stack or nil if the pointer is null.
inline void push_lightuserdata(lua_State* L, void* p) {
	if (p) {
		lua_pushlightuserdata(L, p);
	} else {
		lua_pushnil(L);
	}
}

/** @} */ // end of doc-group lib_core_lua

} // namespace lua
} // namespace togo
