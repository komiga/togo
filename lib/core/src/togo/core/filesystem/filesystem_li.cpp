#line 2 "togo/core/filesystem/filesystem_li.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/filesystem/filesystem.hpp>
#include <togo/core/filesystem/directory_reader.hpp>
#include <togo/core/lua/lua.hpp>

namespace togo {

namespace directory_reader {

TOGO_LI_FUNC_DEF(destroy) {
	auto& reader = *lua::get_userdata<DirectoryReader>(L, 1);
	reader.~DirectoryReader();
	return 0;
}

} // namespace directory_reader

namespace filesystem {

TOGO_LI_FUNC_DEF(iterate_dir_iter) {
	auto& reader = *lua::get_userdata<DirectoryReader>(L, 1);
	auto type_mask = static_cast<DirectoryEntry::Type>(lua::get_integer(L, lua_upvalueindex(1)));
	DirectoryEntry entry{};
	if (directory_reader::read(reader, entry, type_mask)) {
		lua::push_value(L, entry.path);
		lua::push_value(L, signed_cast(entry.type));
		return 2;
	}
	return 0;
}

TOGO_LI_FUNC_DEF(iterate_dir) {
	auto path = lua::get_string(L, 1);
	auto type_mask = lua::get_integer(L, 2);
	bool recursive = luaL_opt(L, lua::get_boolean, 3, true);
	bool ignore_dotfiles = luaL_opt(L, lua::get_boolean, 4, false);

	lua::push_value(L, type_mask);
	lua_pushcclosure(L, TOGO_LI_FUNC(iterate_dir_iter), 1);
	auto& reader = *lua::new_userdata<DirectoryReader>(L);
	lua::push_value(L, signed_cast(type_mask));
	if (!directory_reader::open(reader, path, recursive, ignore_dotfiles)) {
		lua_pop(L, 3);
		return 0;
	}
	return 3;
}

} // namespace filesystem

static luaL_reg const li_funcs[]{
	TOGO_LI_FUNC_REF(filesystem, iterate_dir)
	{nullptr, nullptr}
};

/// Register the Lua interface.
void filesystem::register_lua_interface(lua_State* L) {
	lua::register_userdata<DirectoryReader>(L, directory_reader::li_destroy);
	luaL_register(L, "togo.filesystem", li_funcs);

	lua_createtable(L, 0, 3);
	lua::table_set_copy_raw(L, -4, "EntryType", -1);
	lua::table_set_raw(L, "file", signed_cast(DirectoryEntry::Type::file));
	lua::table_set_raw(L, "dir" , signed_cast(DirectoryEntry::Type::dir));
	lua::table_set_raw(L, "all" , signed_cast(DirectoryEntry::Type::all));
	lua_pop(L, 1);

	lua_pop(L, 1); // module table
}

} // namespace togo
