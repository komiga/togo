
function print_preload()
	print("preload:")
	for k, v in pairs(package.preload) do
		print("  " .. k .. " = " .. type(v))
	end
end

print_preload()

local U = require "togo.Util"

U.log("util test")

print_preload()

