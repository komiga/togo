
local U = require "togo.utility"
local IO = require "togo.io"

do
	local test_file = "data/lua_io"
	local test_data = [[test]]

	IO.write_file(test_file, test_data)
	local data = IO.read_file(test_file)

	U.assert(data == test_data)
end
