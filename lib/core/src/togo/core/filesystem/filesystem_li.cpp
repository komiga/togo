#line 2 "togo/core/filesystem/filesystem_li.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
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

TOGO_LI_FUNC_DEF(working_dir) {
	FixedArray<char, 256> path{};
	filesystem::working_dir(path);
	lua::push_value(L, path);
	return 1;
}

TOGO_LI_FUNC_DEF(set_working_dir) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, filesystem::set_working_dir(path));
	return 1;
}

TOGO_LI_FUNC_DEF(exists) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, filesystem::exists(path));
	return 1;
}

TOGO_LI_FUNC_DEF(is_file) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, filesystem::is_file(path));
	return 1;
}

TOGO_LI_FUNC_DEF(is_directory) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, filesystem::is_directory(path));
	return 1;
}

TOGO_LI_FUNC_DEF(is_empty_directory) {
	auto path = lua::get_string(L, 1);
	bool accept_nonexistent = luaL_opt(L, lua::get_boolean, 2, false);
	lua::push_value(L, filesystem::is_empty_directory(path, accept_nonexistent));
	return 1;
}

TOGO_LI_FUNC_DEF(time_last_modified) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, filesystem::time_last_modified(path));
	return 1;
}

TOGO_LI_FUNC_DEF(file_size) {
	auto path = lua::get_string(L, 1);
	lua::push_value(L, filesystem::file_size(path));
	return 1;
}

TOGO_LI_FUNC_DEF(create_file) {
	auto path = lua::get_string(L, 1);
	bool overwrite = luaL_opt(L, lua::get_boolean, 2, false);
	lua::push_value(L, filesystem::create_file(path, overwrite));
	return 1;
}

TOGO_LI_FUNC_DEF(remove_file) {
	auto path = lua::get_string(L, 1);
	bool accept_nonexistent = luaL_opt(L, lua::get_boolean, 2, false);
	lua::push_value(L, filesystem::remove_file(path, accept_nonexistent));
	return 1;
}

TOGO_LI_FUNC_DEF(move_file) {
	auto src = lua::get_string(L, 1);
	auto dest = lua::get_string(L, 2);
	lua::push_value(L, filesystem::move_file(src, dest));
	return 1;
}

TOGO_LI_FUNC_DEF(copy_file) {
	auto src = lua::get_string(L, 1);
	auto dest = lua::get_string(L, 2);
	bool overwrite = luaL_opt(L, lua::get_boolean, 3, false);
	lua::push_value(L, filesystem::copy_file(src, dest, overwrite));
	return 1;
}

TOGO_LI_FUNC_DEF(create_directory) {
	auto path = lua::get_string(L, 1);
	bool accept_exists = luaL_opt(L, lua::get_boolean, 2, false);
	lua::push_value(L, filesystem::create_directory(path, accept_exists));
	return 1;
}

TOGO_LI_FUNC_DEF(remove_directory) {
	auto path = lua::get_string(L, 1);
	bool accept_nonexistent = luaL_opt(L, lua::get_boolean, 2, false);
	lua::push_value(L, filesystem::remove_directory(path, accept_nonexistent));
	return 1;
}

// -> entry.path, entry.type
TOGO_LI_FUNC_DEF(iterate_dir_iter) {
	auto& reader = *lua::get_userdata<DirectoryReader>(L, 1);
	auto type_mask = static_cast<DirectoryEntry::Type>(lua::get_integer(L, lua_upvalueindex(1)));
	DirectoryEntry entry{};
	if (directory_reader::read(reader, entry, type_mask)) {
		lua::push_value(L, entry.path);
		lua::push_value(L, unsigned_cast(entry.type));
		return 2;
	}
	directory_reader::close(reader);
	return 0;
}

// Iterate over a directory.
TOGO_LI_FUNC_DEF(iterate_dir) {
	auto path = lua::get_string(L, 1);
	auto type_mask = lua::get_integer(L, 2);
	bool prepend_path = luaL_opt(L, lua::get_boolean, 3, true);
	bool recursive = luaL_opt(L, lua::get_boolean, 4, true);
	bool ignore_dotfiles = luaL_opt(L, lua::get_boolean, 5, false);

	lua::push_value(L, type_mask);
	lua_pushcclosure(L, TOGO_LI_FUNC(iterate_dir_iter), 1);
	auto& reader = *lua::new_userdata<DirectoryReader>(L);
	if (!directory_reader::open(reader, path, prepend_path, recursive, ignore_dotfiles)) {
		lua_pop(L, 2);
		return luaL_error(L, "failed to open directory for reading: %s", path.data);
	}
	return 2;
}

TOGO_LI_FUNC_DEF(__module_init__) {
	lua::register_userdata<DirectoryReader>(L, directory_reader::li_destroy);

	lua_createtable(L, 0, 3);
	lua::table_set_copy_raw(L, 1, "EntryType", -1);
	lua::table_set_raw(L, "file", unsigned_cast(DirectoryEntry::Type::file));
	lua::table_set_raw(L, "dir" , unsigned_cast(DirectoryEntry::Type::dir));
	lua::table_set_raw(L, "all" , unsigned_cast(DirectoryEntry::Type::all));
	lua_pop(L, 1);

	return 0;
}

} // namespace filesystem

namespace {

static LuaModuleFunctionArray const li_funcs{
	TOGO_LI_FUNC_REF(filesystem, __module_init__)

	TOGO_LI_FUNC_REF(filesystem, working_dir)
	TOGO_LI_FUNC_REF(filesystem, set_working_dir)
	TOGO_LI_FUNC_REF(filesystem, exists)
	TOGO_LI_FUNC_REF(filesystem, is_file)
	TOGO_LI_FUNC_REF(filesystem, is_directory)
	TOGO_LI_FUNC_REF(filesystem, is_empty_directory)
	TOGO_LI_FUNC_REF(filesystem, time_last_modified)
	TOGO_LI_FUNC_REF(filesystem, file_size)
	TOGO_LI_FUNC_REF(filesystem, create_file)
	TOGO_LI_FUNC_REF(filesystem, remove_file)
	TOGO_LI_FUNC_REF(filesystem, move_file)
	TOGO_LI_FUNC_REF(filesystem, copy_file)
	TOGO_LI_FUNC_REF(filesystem, create_directory)
	TOGO_LI_FUNC_REF(filesystem, remove_directory)

	TOGO_LI_FUNC_REF(filesystem, iterate_dir)
};

static LuaModuleRef const li_module{
	"togo.filesystem",
	"togo/core/filesystem/filesystem.lua",
	li_funcs,
	#include <togo/core/filesystem/filesystem.lua>
};

} // anonymous namespace

/// Register the Lua interface.
void filesystem::register_lua_interface(lua_State* L) {
	lua::preload_module(L, li_module);
}

} // namespace togo
