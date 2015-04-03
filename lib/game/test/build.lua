
local S, G, R = precore.helpers()

local configs = {
	"togo.lib.game.dep",
}

togo.make_tests("app", {
	["general"] = {nil, configs},
})

togo.make_tests("general", {
	["headers"] = {nil, configs},
})

togo.make_tests("gfx", {
	["display"] = {nil, configs},
	["renderer_triangle"] = {nil, configs},
	["renderer_pipeline"] = {nil, configs},
})

togo.make_tests("resource", {
	["general"] = {nil, configs},
	["manager"] = {nil, configs},
})
