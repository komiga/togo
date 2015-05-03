
local S, G, R = precore.helpers()

precore.make_config("togo.lib.game.dep", {
	reverse = true,
}, {
"togo.base",
"togo.lib.core.dep",
"togo.lib.window.dep",
{project = function(p)
	togo.library_config("game")

	configuration {"togo-test"}
		defines {
			-- "TOGO_TEST_",
		}
end}})

precore.append_config_scoped("togo.projects", {
{global = function(_)
	togo.make_library("game", {
		"togo.lib.game.dep",
	})
end}})
