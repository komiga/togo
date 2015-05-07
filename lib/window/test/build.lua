
local S, G, R = precore.helpers()

local configs = {
	"togo.lib.window.dep",
}

togo.make_tests("general", {
	["headers"] = {nil, configs},
})

togo.make_tests("window", {
	["raster"] = {nil, configs},
	["opengl"] = {nil, configs},
})
