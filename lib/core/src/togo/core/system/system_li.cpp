#line 2 "togo/core/system/filesystem_li.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/lua/lua.hpp>

namespace togo {

namespace system {

TOGO_LI_FUNC_DEF(pid) {
	lua::push_value(L, system::pid());
	return 1;
}

TOGO_LI_FUNC_DEF(num_cores) {
	lua::push_value(L, system::num_cores());
	return 1;
}

TOGO_LI_FUNC_DEF(hostname) {
	lua::push_value(L, system::hostname());
	return 1;
}

TOGO_LI_FUNC_DEF(sleep_ms) {
	auto duration_ms = lua::get_integer(L, 1);
	system::sleep_ms(duration_ms);
	return 0;
}

TOGO_LI_FUNC_DEF(secs_since_epoch) {
	lua::push_value(L, system::secs_since_epoch());
	return 1;
}

TOGO_LI_FUNC_DEF(time_monotonic) {
	lua::push_value(L, system::time_monotonic());
	return 1;
}

TOGO_LI_FUNC_DEF(environment_variable) {
	auto name = lua::get_string(L, 1);
	lua::push_value(L, system::environment_variable(name));
	return 1;
}

TOGO_LI_FUNC_DEF(set_environment_variable) {
	auto name = lua::get_string(L, 1);
	auto value = lua::get_string(L, 2);
	lua::push_value(L, system::set_environment_variable(name, value));
	return 1;
}

TOGO_LI_FUNC_DEF(remove_environment_variable) {
	auto name = lua::get_string(L, 1);
	lua::push_value(L, system::remove_environment_variable(name));
	return 1;
}

} // namespace system

namespace {

static LuaModuleFunctionArray const li_funcs{
	TOGO_LI_FUNC_REF(system, pid)
	TOGO_LI_FUNC_REF(system, num_cores)
	TOGO_LI_FUNC_REF(system, hostname)
	TOGO_LI_FUNC_REF(system, sleep_ms)
	TOGO_LI_FUNC_REF(system, secs_since_epoch)
	TOGO_LI_FUNC_REF(system, time_monotonic)
	TOGO_LI_FUNC_REF(system, environment_variable)
	TOGO_LI_FUNC_REF(system, set_environment_variable)
	TOGO_LI_FUNC_REF(system, remove_environment_variable)
};

static LuaModuleRef const li_module{
	"togo.system",
	"togo/core/system/system.lua",
	li_funcs,
	#include <togo/core/system/system.lua>
};

} // anonymous namespace

/// Register the Lua interface.
void system::register_lua_interface(lua_State* L) {
	lua::preload_module(L, li_module);
}

} // namespace togo
