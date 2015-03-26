
-- Import precore

function import_precore()
	local env_name = "PRECORE_ROOT"
	local root_path = os.getenv(env_name)
	if nil == root_path or 0 == #root_path then
		error(
			"Environment variable '" .. env_name .. "' is not " ..
			"defined or is blank; assign it to precore's root directory"
		)
	end
	dofile(path.join(root_path, "precore.lua"))
end

import_precore()

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
			-- precore.subst("${ROOT}/dep/luajit/include/luajit-2.0/"),
		}

	-- premake4.4-beta5 doesn't have the "opt=value" syntax for configuration
	-- selection. TODO: Fix these yucklings when there's a release that does.
	configuration {"opengl"}
		defines {
			"TOGO_CONFIG_RENDERER=TOGO_RENDERER_OPENGL",
		}
		defines {"GLEW_STATIC"}
		includedirs {
			precore.subst("${ROOT}/dep/glew/include/"),
		}

	configuration {"sdl"}
		defines {
			"TOGO_CONFIG_GRAPHICS_BACKEND=TOGO_GRAPHICS_BACKEND_SDL",
		}
		includedirs {
			precore.subst("${ROOT}/dep/sdl/include/"),
		}

	configuration {"glfw"}
		defines {
			"TOGO_CONFIG_GRAPHICS_BACKEND=TOGO_GRAPHICS_BACKEND_GLFW",
		}
		includedirs {
			precore.subst("${ROOT}/dep/glfw/include/"),
		}
end}})

precore.make_config(
"togo-deps-link", {{
project = function()
	--[[configuration {}
		libdirs {
			precore.subst("${ROOT}/dep/luajit/lib/"),
		}
		links {
			":libluajit-5.1.a",
		}--]]

	configuration {"linux"}
		buildoptions {
			"-pthread",
		}
		links {
			"m",
			"dl",
			"pthread",
		}

	configuration {"opengl"}
		libdirs {
			precore.subst("${ROOT}/dep/glew/lib/"),
		}
		links {
			"GL",
			":libGLEW.a",
		}

	configuration {"sdl"}
		libdirs {
			precore.subst("${ROOT}/dep/sdl/lib/"),
		}
		links {
			"m",
			"dl",
			":libSDL2.a",
		}

	configuration {"glfw", "linux"}
		linkoptions {
			"`pkg-config --static --libs " ..
			precore.subst("${ROOT}/dep/glfw/lib/pkgconfig/glfw3.pc") ..
			"`"
		}

	configuration {"glfw"}
		libdirs {
			precore.subst("${ROOT}/dep/glfw/lib/"),
		}
		links {
			":libglfw3.a",
		}
end}})

precore.make_config(
"togo-config", {{
project = function()
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
		description = "enable test mode",
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
			"TOGO_TEST_INPUT_BUFFER",
		}
end})

precore.make_config(
"opt-togo-renderer", {{
option = {
	data = {
		trigger = "togo-renderer",
		value = "RENDERER",
		description = "select the renderer",
		allowed = {
			{"opengl", "OpenGL renderer"},
		}
	},
	init_handler = function()
		if not _OPTIONS["togo-renderer"] then
			_OPTIONS["togo-renderer"] = "opengl"
		end
	end
}}})

precore.make_config(
"opt-togo-gfx-backend", {{
option = {
	data = {
		trigger = "togo-gfx-backend",
		value = "BACKEND",
		description = "select the graphics system backend",
		allowed = {
			{"sdl", "SDL2 backend"},
			{"glfw", "GLFW backend"},
		}
	},
	init_handler = function()
		if not _OPTIONS["togo-gfx-backend"] then
			_OPTIONS["togo-gfx-backend"] = "glfw"
		end
	end
}}})

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
			precore.subst("${ROOT}/build/lib/")
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
		"opt-togo-renderer",
		"opt-togo-gfx-backend",
	}
)
