#line 2 "togo/core/tool/tool.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/tool/tool.hpp>
#include <togo/core/lua/lua.hpp>

namespace togo {

/// Run tool.
///
/// Uses Tool.run() with the top of the stack (tool_supplied) or Tool.base_tool.
///
/// args should include argv[0] (i.e., use the whole array).
bool tool::run(
	lua_State* L,
	ArrayRef<char const* const> args,
	bool tool_supplied
) {
	lua::push_value(L, lua::pcall_error_message_handler);
	lua::load_module(L, "togo.Tool", true);
	lua::table_get_raw(L, "run");

	if (tool_supplied) {
		// move supplied tool
		lua_pushvalue(L, -4);
		lua_remove(L, -5);
	} else {
		lua::table_get_raw(L, -2, "base_tool");
	}
	lua_remove(L, -3); // togo.Tool

	lua_createtable(L, 0, args.size());
	for (unsigned i = 0; i < args.size(); ++i) {
		lua::table_set_index_raw(L, i + 1, StringRef{args[i], cstr_tag{}});
	}

	bool r = true;
	if (lua_pcall(L, 2, 1, -4)) {
		auto error = lua::get_string(L, -1);
		TOGO_LOGF("error: %.*s\n", error.size, error.data);
		r = false;
	} else if (lua_isboolean(L, -1)) {
		r = lua::get_boolean(L, -1);
	}
	lua_pop(L, 2);

	return r;
}

} // namespace togo
