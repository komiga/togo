
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/filesystem/filesystem.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/lua/lua.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main(signed argc, char* argv[]) {
	memory_init();
	if (argc == 1) {
		TOGO_LOG("usage: general.elf script [...]");
		return 1;
	}

	lua_State* L = lua::new_state();
	TOGO_ASSERTE(L);
	luaL_openlibs(L);
	lua::register_core(L);
	filesystem::register_lua_interface(L);
	io::register_lua_interface(L);

	lua::push_value(L, lua::pcall_error_message_handler);
	for (signed i = 1; i < argc; ++i) {
		StringRef path{argv[i], cstr_tag{}};
		TOGO_ASSERTF(
			filesystem::is_file(path),
			"path is either not a file or does not exist: %.*s\n",
			path.size, path.data
		);

		if (luaL_loadfile(L, path.data)) {
			auto error = lua::get_string(L, -1);
			TOGO_LOGF("failed to load script: %.*s\n", error.size, error.data);
			lua_pop(L, 1);
			continue;
		}
		if (lua_pcall(L, 0, 0, -2)) {
			auto error = lua::get_string(L, -1);
			TOGO_LOGF("script error: %.*s\n", error.size, error.data);
			lua_pop(L, 1);
			continue;
		}
	}
	lua_pop(L, 1);
	lua_close(L);
	return 0;
}
