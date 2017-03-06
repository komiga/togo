
local S, G, R = precore.helpers()

precore.make_config("togo.lib.core.lua.dep", nil, {
{project = function(p)
	configuration {}
		includedirs {
			G"${DEP_PATH}/lua/include/",
		}

	if not precore.env_project()["NO_LINK"] then
		libdirs {
			G"${DEP_PATH}/lua/lib/",
		}
		linkoptions {
			"-Wl,-E -l:liblua.a",
		}
	end
end}})

precore.make_config("togo.lib.core.dep", {
	reverse = true,
}, {
"togo.base",
"togo.lib.core.lua.dep",
{project = function(p)
	togo.library_config("core")

	configuration {"togo-test"}
		defines {
			"TOGO_TEST_PRIORITY_QUEUE",
			"TOGO_TEST_TASK_MANAGER",
			"TOGO_TEST_ALGORITHM",
		}
end}})

precore.append_config_scoped("togo.projects", {
{global = function(_)
	togo.make_library("core", {
		"togo.lib.core.dep",
	})
end}})
