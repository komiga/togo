#line 2 "togo/core/io/io_li.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/file_stream.hpp>
#include <togo/core/filesystem/filesystem.hpp>
#include <togo/core/lua/lua.hpp>

namespace togo {

namespace io {

// Read a file into a string.
TOGO_LI_FUNC_DEF(read_file) {
	auto path = lua::get_string(L, 1);

	FileReader stream;
	if (!stream.open(path)) {
		return luaL_error(L, "failed to open file for reading: %.*s", path.size, path.data);
	}
	u64 remaining = filesystem::file_size(path);
	Array<char> data{memory::scratch_allocator()};
	array::resize(data, remaining);
	auto p = begin(data);
	unsigned read_size;
	while (remaining) {
		if (!io::read(stream, p, remaining, &read_size)) {
			return luaL_error(L, "failed to read chunk from file: %.*s", path.size, path.data);
		}
		remaining -= read_size;
		p += read_size;
	}
	stream.close();

	lua::push_value(L, StringRef{data});
	return 1;
}

// Write a string to a file.
TOGO_LI_FUNC_DEF(write_file) {
	auto path = lua::get_string(L, 1);
	auto data = lua::get_string(L, 2);

	FileWriter stream;
	if (!stream.open(path, false)) {
		return luaL_error(L, "failed to open file for writing: %.*s", path.size, path.data);
	}
	if (!io::write(stream, data.data, data.size)) {
		return luaL_error(L, "failed to write file: %.*s", path.size, path.data);
	}
	stream.close();
	return 0;
}

} // namespace io

static luaL_reg const li_funcs[]{
	TOGO_LI_FUNC_REF(io, read_file)
	TOGO_LI_FUNC_REF(io, write_file)

	{nullptr, nullptr}
};

/// Register the Lua interface.
void io::register_lua_interface(lua_State* L) {
	luaL_register(L, "togo.io", li_funcs);
	lua_pop(L, 1); // module table
}

} // namespace togo

