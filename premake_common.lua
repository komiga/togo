
-- Import precore

local env_name = "PRECORE_ROOT"
local root_path = os.getenv(env_name)

if nil == root_path or 0 == #root_path then
	error(
		"Environment variable '" .. env_name .. "' is not " ..
		"defined or is blank; assign it to precore's root directory"
	)
end

dofile(path.join(os.getenv(env_name), "precore.lua"))

-- Utilities

function action_clean()
	if "clean" == _ACTION then
		for _, pc_sol in pairs(precore.state.solutions) do
			for _, pc_proj in pairs(pc_sol.projects) do
				os.rmdir(path.join(pc_proj.obj.basedir, "out"))
			end
		end
	end
end

-- Custom precore configs

precore.make_config(
"togo-strict", {{
project = function()
	-- NB: -Werror is a pita for GCC. Good for testing, though,
	-- since its error checking is better.
	configuration {"clang"}
		flags {
			"FatalWarnings"
		}

	configuration {"linux"}
		buildoptions {
			"-pedantic-errors",
			"-Wextra",

			"-Wuninitialized",
			"-Winit-self",

			"-Wmissing-field-initializers",
			"-Wredundant-decls",

			"-Wfloat-equal",
			"-Wold-style-cast",

			"-Wnon-virtual-dtor",
			"-Woverloaded-virtual",

			"-Wunused",
			"-Wundef",
		}
end}})

precore.make_config(
"togo-deps", {{
project = function()
	configuration {}
		includedirs {
			--precore.subst("${ROOT}/dep/duct/"),
			precore.subst("${ROOT}/dep/am/"),
		}
end}})

precore.make_config(
"togo-config", {{
project = function()
	configuration {}

	configuration {"debug"}
		defines {
			"TOGO_DEBUG",
			"TOGO_USE_CONSTRAINTS"
		}
end}})

precore.make_config(
"togo-import", {
-- Import togo-config
"togo-config", {
project = function()
	configuration {}
		includedirs {
			precore.subst("${ROOT}/src/")
		}
		libdirs {
			precore.subst("${ROOT}/lib/")
		}

	configuration {"debug"}
		links {"togo_d"}

	configuration {"release"}
		links {"togo"}
end}})

-- Initialize precore

precore.init(
	nil,
	{
		"opt-clang",
		"c++11-core",
		"precore-env-root"
	}
)
