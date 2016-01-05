
local U = require "togo.utility"
local IO = require "togo.io"

do
	local inaccessible_path = "inaccessible/path"
	local test_path = "data/lua_io"
	local test_data = [[test]]

	U.assert(IO.write_file(inaccessible_path, test_data) == false)
	U.assert(IO.read_file(inaccessible_path) == nil)

	U.assert(IO.write_file(test_path, test_data) == true)
	U.assert(IO.read_file(test_path) == test_data)
end
