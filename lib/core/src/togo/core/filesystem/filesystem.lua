u8R""__RAW_STRING__(

local U = require "togo.utility"
local M = U.module(...)

M.debug = false

function M.working_dir_scope(path, f)
	U.type_assert(path, "string")
	U.type_assert(f, "function")

	local previous_path = M.working_dir()
	U.assert(previous_path ~= "")
	U.assert(M.set_working_dir(path))
	f(previous_path, path)
	U.assert(M.set_working_dir(previous_path))
end

return M

)"__RAW_STRING__"