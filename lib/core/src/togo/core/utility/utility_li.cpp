#line 2 "togo/core/utility/utility_li.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/utility/args.hpp>
#include <togo/core/memory/temp_allocator.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/lua/lua.hpp>

namespace togo {

namespace utility {

namespace {

// TODO: Yuck! Just expose the KVS interface

static void copy_kvs_to_table(lua_State* L, KVS const& k_list) {
	lua_createtable(L, 1, kvs::size(k_list));
	lua::table_set_raw(L, "name", kvs::name_ref(k_list));
	signed i = 1;
	for (auto const& k_value : k_list) {
		lua_createtable(L, 2, 0);
		lua::table_set_raw(L, "name", kvs::name_ref(k_value));
		switch (kvs::type(k_value)) {
		case KVSType::boolean: lua::table_set_raw(L, "value", kvs::boolean(k_value)); break;
		case KVSType::string: lua::table_set_raw(L, "value", kvs::string_ref(k_value)); break;
		default:
			TOGO_ASSERT(false, "missing type handler");
		}
		lua_rawseti(L, -2, i++);
	}
}

} // anonymous namespace

TOGO_LI_FUNC_DEF(parse_args) {
	luaL_checktype(L, 1, LUA_TTABLE);
	TempAllocator<sizeof(char*) * 64> allocator;
	Array<char const*> argv{allocator};
	for (signed i = 1; (lua_rawgeti(L, 1, i), lua_type(L, -1) != LUA_TNIL); ++i) {
		array::push_back(argv, lua::get_string(L, -1).data);
		lua_pop(L, 1);
	}

	KVS k_options{};
	KVS k_command_options{};
	KVS k_command{};
	bool command_parsed = parse_args(k_options, k_command_options, k_command, array::size(argv), begin(argv));

	lua::push_value(L, command_parsed);
	copy_kvs_to_table(L, k_options);
	copy_kvs_to_table(L, k_command_options);
	copy_kvs_to_table(L, k_command);
	return 4;
}

} // namespace utility

namespace string {

TOGO_LI_FUNC_DEF(path_dir) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, string::path_dir(path));
	return 1;
}

TOGO_LI_FUNC_DEF(path_file) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, string::path_file(path));
	return 1;
}

} // namespace string

namespace {

static LuaModuleFunctionArray const li_funcs{
	TOGO_LI_FUNC_REF(utility, parse_args)

	TOGO_LI_FUNC_REF(string, path_dir)
	TOGO_LI_FUNC_REF(string, path_file)
};

static LuaModuleRef const li_module{
	"togo.utility",
	"togo/core/utility/utility.lua",
	li_funcs,
	#include <togo/core/utility/utility.lua>
};

} // anonymous namespace

/// Register the Lua interface.
void utility::register_lua_interface(lua_State* L) {
	lua::preload_module(L, li_module);
}

} // namespace togo
