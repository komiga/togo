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
#include <togo/core/utility/traits.hpp>
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

/// Push nil to the stack.
inline void push_value(lua_State* L, null_tag const) {
	lua_pushnil(L);
}

/// Push an integral value to the stack.
template<class T>
inline enable_if<is_integral<T>::value, void> push_value(lua_State* L, T value) {
	lua_pushinteger(L, value);
}

/// Get an argument from the stack as an integer.
inline LuaInt get_integer(lua_State* L, signed narg) {
	return luaL_checkinteger(L, narg);
}

/// Push a floating-point value to the stack.
template<class T>
inline enable_if<is_floating_point<T>::value, void> push_value(lua_State* L, T value) {
	lua_pushinteger(L, value);
}

/// Get an argument from the stack as an integer.
inline LuaFloat get_float(lua_State* L, signed narg) {
	return luaL_checknumber(L, narg);
}

// NB: Using template to avoid implicit cast from pointers and integrals.
/// Push a bool to the stack.
template<class T>
inline enable_if<is_same<T, bool>::value, void> push_value(lua_State* L, T value) {
	lua_pushboolean(L, value);
}

/// Get an argument from the stack as a boolean.
inline bool get_boolean(lua_State* L, signed narg) {
	luaL_checktype(L, narg, LUA_TBOOLEAN);
	return lua_toboolean(L, narg);
}

/// Push a string to the stack.
inline void push_value(lua_State* L, StringRef str) {
	lua_pushlstring(L, str.data ? str.data : "", unsigned_cast(str.size));
}

/// Get a string from the stack as a string reference.
inline StringRef get_string(lua_State* L, signed narg) {
	size_t size = 0;
	auto data = luaL_checklstring(L, narg, &size);
	return {data, static_cast<unsigned>(size)};
}

/// Push a C function to the stack.
inline void push_value(lua_State* L, LuaCFunction value) {
	lua_pushcfunction(L, value);
}

/// Get a C function from the stack.
inline LuaCFunction* get_cfunction(lua_State* L, signed narg) {
	luaL_checktype(L, narg, LUA_TFUNCTION);
	return lua_tocfunction(L, narg);
}

/// Set table[name] = value (with settable).
template<class T>
inline void table_set(lua_State* L, signed table, StringRef name, T value) {
	lua::push_value(L, name);
	lua::push_value(L, value);
	lua_settable(L, table);
}

/// Set top_of_stack[name] = value (with settable).
template<class T>
inline void table_set(lua_State* L, StringRef name, T value) {
	lua::table_set(L, -3, name, value);
}

/// Set table[name] = value (with rawset).
template<class T>
inline void table_set_raw(lua_State* L, signed table, StringRef name, T value) {
	lua::push_value(L, name);
	lua::push_value(L, value);
	lua_rawset(L, table);
}

/// Set top_of_stack[name] = value (with rawset).
template<class T>
inline void table_set_raw(lua_State* L, StringRef name, T value) {
	lua::table_set_raw(L, -3, name, value);
}

/// Fetch the value of table[name] and push it on the stack (with gettable).
inline void table_get(lua_State* L, signed table, StringRef name) {
	lua::push_value(L, name);
	lua_gettable(L, table);
}

/// Fetch the value of top_of_stack[name] and push it on the stack (with gettable).
inline void table_get(lua_State* L, StringRef name) {
	lua::table_get(L, -2, name);
}

/// Fetch the value of table[name] and push it on the stack (with rawget).
inline void table_get_raw(lua_State* L, signed table, StringRef name) {
	lua::push_value(L, name);
	lua_rawget(L, table);
}

/// Fetch the value of top_of_stack[name] and push it on the stack (with rawget).
inline void table_get_raw(lua_State* L, StringRef name) {
	lua::table_get_raw(L, -2, name);
}

/// Set table[index] = value (with settable).
template<class T>
inline void table_set_index(lua_State* L, signed table, signed index, T value) {
	lua::push_value(L, index);
	lua::push_value(L, value);
	lua_settable(L, table);
}

/// Set top_of_stack[index] = value (with settable).
template<class T>
inline void table_set_index(lua_State* L, signed index, T value) {
	lua::table_set_index(L, -3, index, value);
}

/// Set table[index] = value (with rawset).
template<class T>
inline void table_set_index_raw(lua_State* L, signed table, signed index, T value) {
	lua::push_value(L, value);
	lua_rawseti(L, table, index);
}

/// Set top_of_stack[index] = value (with rawset).
template<class T>
inline void table_set_index_raw(lua_State* L, signed index, T value) {
	lua::table_set_index_raw(L, -2, index, value);
}

/// Fetch the value of table[index] and push it on the stack (with rawget).
inline void table_get_index(lua_State* L, signed table, signed index) {
	lua::push_value(L, index);
	lua_gettable(L, table);
}

/// Fetch the value of top_of_stack[index] and push it on the stack (with rawget).
inline void table_get_index(lua_State* L, signed index) {
	lua::table_get_index(L, -2, index);
}

/// Fetch the value of table[index] and push it on the stack (with rawget).
inline void table_get_index_raw(lua_State* L, signed table, signed index) {
	lua_rawgeti(L, table, index);
}

/// Fetch the value of top_of_stack[index] and push it on the stack (with rawget).
inline void table_get_index_raw(lua_State* L, signed index) {
	lua::table_get_index_raw(L, -1, index);
}

/// Set table[name] = stack[value] (with settable).
inline void table_set_copy(lua_State* L, signed table, StringRef name, signed value) {
	lua::push_value(L, name);
	lua_pushvalue(L, value - 1);
	lua_settable(L, table);
}

/// Set top_of_stack[name] = stack[value] (with settable).
inline void table_set_copy(lua_State* L, StringRef name, signed value) {
	lua::table_set_copy(L, -3, name, value);
}

/// Set table[name] = stack[value] (with rawset).
inline void table_set_copy_raw(lua_State* L, signed table, StringRef name, signed value) {
	lua::push_value(L, name);
	lua_pushvalue(L, value - 1);
	lua_rawset(L, table);
}

/// Set top_of_stack[name] = stack[value] (with rawset).
inline void table_set_copy_raw(lua_State* L, StringRef name, signed value) {
	lua::table_set_copy_raw(L, -3, name, value);
}

/// Set table[index] = stack[value] (with settable).
inline void table_set_copy_index(lua_State* L, signed table, signed index, signed value) {
	lua::push_value(L, index);
	lua_pushvalue(L, value - 1);
	lua_settable(L, table);
}

/// Set top_of_stack[index] = stack[value] (with settable).
inline void table_set_copy_index(lua_State* L, signed index, signed value) {
	lua::table_set_copy_index(L, -3, index, value);
}

/// Set table[index] = stack[value] (with rawset).
inline void table_set_copy_index_raw(lua_State* L, signed table, signed index, signed value) {
	lua_pushvalue(L, value);
	lua_rawseti(L, table, index);
}

/// Set top_of_stack[index] = stack[value] (with rawset).
inline void table_set_copy_index_raw(lua_State* L, signed index, signed value) {
	lua::table_set_copy_index_raw(L, -2, index, value);
}

template<class T>
inline void register_userdata(lua_State* L, LuaCFunction destroy) {
	lua::table_get_raw(L, LUA_REGISTRYINDEX, "togo_class");
	TOGO_ASSERT(!lua_isnil(L, -1), "core was not registered");

	lua::table_get_raw(L, T::lua_metatable_name);
	TOGO_ASSERT(lua_isnil(L, -1), "class metatable was already registered");
	lua_pop(L, 1);

	lua_createtable(L, 0, 1);
	lua::table_set_raw(L, "__gc", destroy);

	// togo_class[T::lua_metatable_name] = metatable
	lua::table_set_copy_raw(L, -4, T::lua_metatable_name, -1);
	lua_pop(L, 2); // metatable, togo_class
}

/// Push a new userdata to the stack and setup its metatable.
template<class T, class... P>
inline T* new_userdata(lua_State* L, P&&... p) {
	auto* ptr = lua_newuserdata(L, sizeof(T));
	TOGO_DEBUG_ASSERTE(ptr);

	lua::table_get_raw(L, LUA_REGISTRYINDEX, "togo_class");
	TOGO_ASSERT(!lua_isnil(L, -1), "core was not registered");
	lua::table_get_raw(L, T::lua_metatable_name);
	TOGO_ASSERT(!lua_isnil(L, -1), "class metatable was not registered");
	lua_setmetatable(L, -3);
	lua_pop(L, 1);

	return new(ptr) T(p...);
}

/// Get an argument from the stack as userdata (typed).
template<class T>
inline T* get_userdata(lua_State* L, signed narg, bool require = true) {
	{auto type = lua_type(L, narg);
	if (type != LUA_TUSERDATA && type != LUA_TNIL) {
		luaL_argerror(L, narg, "expected userdata");
	}}
	auto p = lua_touserdata(L, narg);
	if (require && !p) {
		luaL_argerror(L, narg, "value must be non-null");
	} else {
		if (!lua_getmetatable(L, narg)) {
			luaL_argerror(L, narg, "expected a metatable for userdata, but it has none");
		}
		lua::table_get_raw(L, LUA_REGISTRYINDEX, "togo_class");
		TOGO_ASSERT(!lua_isnil(L, -1), "core was not registered");
		lua::table_get_raw(L, T::lua_metatable_name);
		TOGO_ASSERT(!lua_isnil(L, -1), "class metatable was not registered");
		if (!lua_rawequal(L, -1, -3)) {
			luaL_argerror(L, narg, "userdata metatable does not match the expected one");
		}
		lua_pop(L, 3);
	}
	return static_cast<T*>(p);
}

/// Push a light userdata to the stack or nil if the pointer is null.
inline void push_lightuserdata(lua_State* L, void* p) {
	if (p) {
		lua_pushlightuserdata(L, p);
	} else {
		lua_pushnil(L);
	}
}

/// Get an argument from the stack as light userdata.
inline void* get_lightuserdata(lua_State* L, signed narg, bool require = true) {
	{auto type = lua_type(L, narg);
	if (type != LUA_TLIGHTUSERDATA && type != LUA_TNIL) {
		luaL_argerror(L, narg, "expected light userdata");
	}}
	auto p = lua_touserdata(L, narg);
	if (require && !p) {
		luaL_argerror(L, narg, "value must be non-null");
	}
	return p;
}

/// Get an argument from the stack as light userdata (typed).
///
/// This is an unchecked cast.
template<class T>
inline T* get_lightuserdata_typed(lua_State* L, signed narg, bool require = true) {
	return static_cast<T*>(get_lightuserdata(L, narg, require));
}

/** @} */ // end of doc-group lib_core_lua

} // namespace lua
} // namespace togo
