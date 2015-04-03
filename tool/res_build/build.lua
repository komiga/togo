
local S, G, R = precore.helpers()

precore.make_config("togo.tool.res_build.dep", {
	reverse = true,
}, {
"togo.base",
"togo.lib.core.dep",
"togo.lib.game.dep",
{project = function(_)
	togo.tool_config("res_build")
end}})

precore.append_config_scoped("togo.projects", {
{global = function(_)
	togo.make_tool("res_build", {
		"togo.tool.res_build.dep",
	})
end}})
