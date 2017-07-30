
local S, G, R = precore.helpers()

precore.make_config("togo.lib.platform.linux.dep", nil, {
{project = function(p)
	if not precore.env_project()["NO_LINK"] then
		configuration {"linux"}
			linkoptions {
				"`pkg-config --libs" ..
				" libnotify" ..
				"`"
			}
	end
	configuration {"linux"}
		buildoptions {
			"`pkg-config --cflags-only-I" ..
			" libnotify" ..
			"`"
		}
end}})

precore.make_config("togo.lib.platform.dep", {
	reverse = true,
}, {
"togo.base",
"togo.lib.core.dep",
"togo.lib.platform.linux.dep",
{project = function(p)
	togo.library_config("platform")

	configuration {"togo-test"}
		defines {
			-- "TOGO_TEST_",
		}
end}})

precore.append_config_scoped("togo.projects", {
{global = function(_)
	togo.make_library("platform", {
		"togo.lib.platform.dep",
	})
end}})
