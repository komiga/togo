
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
			"FatalWarnings",
		}
		buildoptions {
			"-Wno-extra-semi",
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
			precore.subst("${ROOT}/dep/am/"),
			precore.subst("${ROOT}/dep/glew/include/"),
			precore.subst("${ROOT}/dep/sdl/include/"),
		}

		defines {"GLEW_STATIC"}
end}})

precore.make_config(
"togo-deps-link", {{
project = function()
	-- Only backend for now, so no magic fluff for actual configuration
	configuration {}
		libdirs {
			precore.subst("${ROOT}/dep/glew/lib/"),
			precore.subst("${ROOT}/dep/sdl/lib/"),
		}

		links {
			"m",
			"dl",
			"GL",
			":libGLEW.a",
			":libSDL2.a",
		}
end}})

precore.make_config(
"togo-config", {{
project = function()
	configuration {}
		defines {
			"TOGO_CONFIG_GRAPHICS_BACKEND=TOGO_GRAPHICS_BACKEND_SDL",
			"TOGO_CONFIG_RENDERER=TOGO_RENDERER_OPENGL",
		}

	configuration {"debug"}
		defines {
			"TOGO_DEBUG",
			"TOGO_USE_CONSTRAINTS",
		}

	configuration {"linux"}
		defines {
			-- Use 64-bit off_t on POSIX.1-2001-compliant systems
			"_FILE_OFFSET_BITS=64"
		}
end}})

precore.make_config(
"opt-togo-test", {{
option = {
	data = {
		trigger = "togo-test",
		description = "whether to use the test config"
	},
	init_handler = function()
	end
}},
function()
	configuration {"togo-test"}
		defines {
			"TOGO_TEST_PRIORITY_QUEUE",
			"TOGO_TEST_TASK_MANAGER",
			"TOGO_TEST_ALGORITHM",
		}
end})

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
		"precore-env-root",
		"opt-togo-test",
	}
)
