u8R""__RAW_STRING__(

local U = require "togo.utility"
local M = U.module(...)

--function M.__module_init__()
	U.set_functable(M, function(_, ...)
		return M.__mm_ctor(...)
	end)
--end

return M

)"__RAW_STRING__"