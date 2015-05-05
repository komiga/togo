
local S, G, R = precore.helpers()

precore.make_config("togo.lib.image.dep", {
	reverse = true,
}, {
"togo.base",
"togo.lib.core.dep",
{project = function(p)
	togo.library_config("image")

	configuration {"togo-test"}
		defines {
			-- "TOGO_TEST_",
		}
end}})

precore.append_config_scoped("togo.projects", {
{global = function(_)
	togo.make_library("image", {
		"togo.lib.image.dep",
	})
end}})
