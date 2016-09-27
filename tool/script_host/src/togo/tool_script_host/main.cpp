#line 2 "togo/tool_script_host/main.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_script_host/config.hpp>
#include <togo/tool_script_host/types.hpp>

#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/filesystem/filesystem.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/lua/lua.hpp>

using namespace togo;

signed main(signed argc, char* argv[]) {
	if (argc == 1) {
		TOGO_LOG("error: expected script path\n");
		return 1;
	}

	StringRef script_path{argv[1], cstr_tag{}};
	if (!filesystem::is_file(script_path)) {
		TOGO_LOGF(
			"error: path is either not a file or does not exist: %.*s\n",
			script_path.size, script_path.data
		);
		return 2;
	}

	signed ec = 0;
	memory::init();

	lua_State* L = lua::new_state();
	TOGO_ASSERTE(L);
	luaL_openlibs(L);

	lua::register_core(L);
	io::register_lua_interface(L);
	filesystem::register_lua_interface(L);

	lua::push_value(L, lua::pcall_error_message_handler);
	if (luaL_loadfile(L, script_path.data)) {
		auto error = lua::get_string(L, -1);
		TOGO_LOGF("failed to load script: %.*s\n", error.size, error.data);
		lua_pop(L, 1);
		ec = 3;
	} else {
		lua_createtable(L, 0, argc - 1);
		for (signed i = 1; i < argc; ++i) {
			lua::table_set_index_raw(L, i, StringRef{argv[i], cstr_tag{}});
		}
		if (lua_pcall(L, 1, 1, -3)) {
			auto error = lua::get_string(L, -1);
			TOGO_LOGF("error: %.*s\n", error.size, error.data);
			ec = 4;
		} else {
			if (lua_isboolean(L, -1) && !lua::get_boolean(L, -1)) {
				ec = 5;
			}
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	lua_close(L);

	memory::shutdown();
	return ec;
}
