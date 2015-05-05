
local S, G, R = precore.helpers()

local configs = {
	"togo.lib.image.dep",
}

togo.make_tests("general", {
	["headers"] = {nil, configs},
})

togo.make_tests("pixmap", {
	["general"] = {nil, configs},
})
