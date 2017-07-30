
local S, G, R = precore.helpers()

local configs = {
	"togo.lib.platform.dep",
}

togo.make_tests("general", {
	["headers"] = {nil, configs},
})

togo.make_tests("notification", {
	["notification"] = {nil, configs},
})
