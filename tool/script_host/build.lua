
local S, G, R = precore.helpers()

precore.make_config("togo.tool.script_host.dep", {
	reverse = true,
}, {
"togo.base",
"togo.lib.core.dep",
"togo.lib.game.dep",
{project = function(_)
	togo.tool_config("script_host")
end}})

precore.append_config_scoped("togo.projects", {
{global = function(_)
	togo.make_tool("script_host", {
		"togo.tool.script_host.dep",
	})
end}})
