
local S, G, R = precore.helpers()

precore.make_config("togo.lib.game.renderer.opt", {
	once = true,
}, {
{option = {
	data = {
		trigger = "togo-game-renderer",
		value = "RENDERER",
		description = "select the renderer",
		allowed = {
			{"opengl", "OpenGL renderer"},
		}
	},
	init_handler = function()
		if not _OPTIONS["togo-game-renderer"] then
			_OPTIONS["togo-game-renderer"] = "opengl"
		end
	end
}}})

precore.make_config("togo.lib.game.gfx-backend.opt", {
	once = true,
}, {
{option = {
	data = {
		trigger = "togo-game-gfx-backend",
		value = "BACKEND",
		description = "select the graphics system backend",
		allowed = {
			{"glfw", "GLFW backend (default)"},
			{"sdl", "SDL2 backend"},
		}
	},
	init_handler = function()
		if not _OPTIONS["togo-game-gfx-backend"] then
			_OPTIONS["togo-game-gfx-backend"] = "glfw"
		end
	end
}}})

precore.make_config("togo.lib.game.renderer.dep", nil, {
{project = function(p)
	assert(_OPTIONS["togo-game-renderer"] == "opengl")
	configuration {"opengl"}
		defines {
			"TOGO_CONFIG_RENDERER=TOGO_RENDERER_OPENGL",
		}
		defines {"GLEW_STATIC"}
		includedirs {
			G"${DEP_PATH}/glew/include/",
		}
		if not precore.env_project()["NO_LINK"] then
			libdirs {
				G"${DEP_PATH}/glew/lib/",
			}
			links {
				"GL",
				":libGLEW.a",
			}
		end
end}})

precore.make_config("togo.lib.game.gfx-backend.dep", nil, {
{project = function(p)
	-- premake4.4-beta5 doesn't have the "opt=value" syntax for configuration
	-- selection. TODO: Fix these yucklings when there's a release that does.
	local backend = _OPTIONS["togo-game-gfx-backend"]
	configuration {}
	if backend == "glfw" then
		defines {
			"TOGO_CONFIG_GRAPHICS_BACKEND=TOGO_GRAPHICS_BACKEND_GLFW",
		}
		includedirs {
			G"${DEP_PATH}/glfw/include/",
		}
		if not precore.env_project()["NO_LINK"] then
			configuration {"linux"}
				linkoptions {
					"`pkg-config --static --libs " ..
					G"${DEP_PATH}/glfw/lib/pkgconfig/glfw3.pc" ..
					"`"
				}
			configuration {}
				libdirs {
					G"${DEP_PATH}/glfw/lib/",
				}
				links {
					":libglfw3.a",
				}
		end
	elseif backend == "sdl" then
		defines {
			"TOGO_CONFIG_GRAPHICS_BACKEND=TOGO_GRAPHICS_BACKEND_SDL",
		}
		includedirs {
			G"${DEP_PATH}/sdl/include/",
		}
		if not precore.env_project()["NO_LINK"] then
			libdirs {
				G"${DEP_PATH}/sdl/lib/",
			}
			links {
				":libSDL2.a",
			}
		end
	end
end}})

precore.make_config("togo.lib.game.dep", {
	reverse = true,
}, {
"togo.base",
"togo.lib.core.dep",
"togo.lib.game.renderer.dep",
"togo.lib.game.gfx-backend.dep",
{project = function(p)
	togo.library_config("game")

	configuration {"togo-test"}
		defines {
			"TOGO_TEST_INPUT_BUFFER",
		}
end}})

precore.append_config_scoped("togo.projects", {
{global = function(_)
	togo.make_library("game", {
		"togo.lib.game.dep",
	})
end}})
