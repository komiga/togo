#line 2 "togo/core/lua/lua.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/lua/lua.hpp>

#include <cstring>

namespace togo {

static void* alloc_func(void* ud, void* ptr, size_t osize, size_t nsize) {
	auto a = static_cast<Allocator*>(ud);
	if (nsize == 0) {
		a->deallocate(ptr);
		return nullptr;
	}

	void* new_ptr = a->allocate(nsize);
	// TODO: does Lua do resize blocks a lot? maybe this should be a nop
	// (allocators don't do reallocs)
	if (osize > 0) {
		std::memcpy(new_ptr, ptr, nsize);
	}
	a->deallocate(ptr);
	return new_ptr;
}

/// Create a new state with the given allocator.
///
/// allocator must exist as long as the state.
lua_State* lua::new_state(Allocator& allocator) {
	return lua_newstate(alloc_func, &allocator);
}

/// Create a new state with Lua's allocator.
lua_State* lua::new_state() {
	return luaL_newstate();
}

/// lua_pcall() error message handler.
///
/// Returns a traceback as a string.
signed lua::pcall_error_message_handler(lua_State* L) {
	if (lua_isstring(L, 1)) {
		luaL_traceback(L, L, lua_tostring(L, 1), 1);
	}
	return 1;
}

namespace {

static LuaModuleRef const li_modules[]{
{
	"togo.Util",
	#include <togo/core/lua/Util.lua>
},
};

static signed package_preloader(lua_State* L) {
	StringRef source = lua::get_string(L, lua_upvalueindex(1));
	StringRef modname = lua::get_string(L, 1);
	lua::push_value(L, lua::pcall_error_message_handler);
	// -> chunk
	if (luaL_loadbufferx(L, source.data, source.size, modname.data, "t")) {
		return lua_error(L);
	}
	// -> return
	if (lua_pcall(L, 0, 1, -2)) {
		return lua_error(L);
	}
	lua_remove(L, -2); // pcall_message_handler

	// remove preloader
	lua::table_get_raw(L, LUA_REGISTRYINDEX, "_PRELOAD");
	lua::table_set_raw(L, modname, null_tag{});
	lua_pop(L, 1); // _PRELOAD
	return 1;
}

} // anonymous namespace

/// Put module in package.preload[modname].
///
/// source must be valid as long as the state is alive.
void lua::preload_module(lua_State* L, LuaModuleRef const& module) {
	lua::table_get_raw(L, LUA_REGISTRYINDEX, "_PRELOAD");
	lua::push_value(L, module.name);
	lua::push_value(L, module.source);
	lua_pushcclosure(L, package_preloader, 1);
	lua_rawset(L, -3); // _PRELOAD[module.name] = closure{package_preloader, module.source}
	lua_pop(L, 1); // _PRELOAD
}

/// Register core interfaces.
void lua::register_core(lua_State* L) {
	lua_createtable(L, 0, 32);
	lua::table_set_copy_raw(L, LUA_REGISTRYINDEX, "togo_class", -1);
	lua_pop(L, 1);

	for (auto& module : li_modules) {
		lua::preload_module(L, module);
	}
}

} // namespace togo
