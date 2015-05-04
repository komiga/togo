
local S, G, R = precore.helpers()

local configs = {
	"togo.lib.window.dep",
}

togo.make_tests("general", {
	["headers"] = {nil, configs},
})

togo.make_tests("window", {
	["window"] = {nil, configs},
	["window_opengl"] = {nil, configs},
})
