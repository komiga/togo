
local S, G, R = precore.helpers()

togo = {}

function togo.library_config(name)
	configuration {}
		includedirs {
			G"${TOGO_ROOT}/lib/" .. name .. "/src/",
		}
	togo.link_library(name)
end

function togo.tool_config(name)
	configuration {}
		includedirs {
			G"${TOGO_ROOT}/tool/" .. name .. "/src/",
		}
	togo.link_library("tool_" .. name)
end

function togo.link_library(name)
	name = "togo_" .. name
	if not precore.env_project()["NO_LINK"] then
		configuration {"release"}
			links {name}

		configuration {"debug"}
			links {name .. "_d"}
	end
end

function togo.make_library(name, configs, env)
	configs = configs or {}
	table.insert(configs, 1, "togo.strict")
	table.insert(configs, 2, "togo.base")

	env = precore.internal.env_set({
		TOGO_LIBRARY = true,
		NO_LINK = true,
	}, env or {})

	local p = precore.make_project(
		"lib_" .. name,
		"C++", "StaticLib",
		"${TOGO_BUILD}/lib/",
		"${TOGO_BUILD}/out/${NAME}/",
		env, configs
	)

	configuration {"debug"}
		targetsuffix("_d")

	configuration {}
		targetname("togo_" .. name)
		files {
			"src/**.cpp",
		}

	if os.isfile("test/build.lua") then
		precore.push_wd("test")
		local prev_solution = solution()
		precore.make_solution(
			"lib_" .. name .. "_test",
			{"debug", "release"},
			{"x64", "x32"},
			nil,
			{
				"precore.generic",
			}
		)
		precore.import(".")
		precore.pop_wd()
		solution(prev_solution.name)
	end
end

function togo.make_tool(name, configs, env)
	configs = configs or {}
	table.insert(configs, 1, "togo.strict")
	table.insert(configs, 2, "togo.base")

	env = env or {}
	local lib_env = precore.internal.env_set({
		TOGO_TOOL_LIB = true,
		NO_LINK = true,
	}, env)
	local lib_proj = precore.make_project(
		"tool_" .. name .. "_lib",
		"C++", "StaticLib",
		"${TOGO_BUILD}/lib/",
		"${TOGO_BUILD}/out/${NAME}/",
		lib_env, configs
	)

	configuration {"debug"}
		targetsuffix("_d")

	configuration {}
		targetname("togo_tool_" .. name)
		files {
			"src/**.cpp",
		}
		excludes {
			"src/**/main.cpp"
		}

	local app_env = precore.internal.env_set({
		TOGO_TOOL = true,
	}, env)
	precore.make_project(
		"tool_" .. name,
		"C++", "ConsoleApp",
		"${TOGO_BUILD}/bin/",
		"${TOGO_BUILD}/out/${NAME}/",
		app_env, configs
	)

	configuration {"linux"}
		targetsuffix(".elf")

	configuration {}
		targetname(name)
		files {
			"src/**/main.cpp",
		}
end

function togo.make_test(group, name, srcglob, configs)
	configs = configs or {}
	table.insert(configs, 1, "togo.strict")
	table.insert(configs, 2, "togo.base")

	local env = {
		TOGO_TEST = true,
	}
	precore.make_project(
		group .. "_" .. name,
		"C++", "ConsoleApp",
		"./",
		"out/",
		env, configs
	)
	if not srcglob then
		srcglob = name .. ".cpp"
	end

	configuration {"linux"}
		targetsuffix(".elf")

	configuration {}
		targetname(name)
		includedirs {
			G"${TOGO_ROOT}/test/support/",
		}
		files {
			srcglob
		}
end

function togo.make_tests(group, tests)
	precore.push_wd(group)
	for name, test in pairs(tests) do
		togo.make_test(group, name, test[1], test[2])
	end
	precore.pop_wd()
end

precore.make_config_scoped("togo.env", {
	once = true,
}, {
{global = function()
	precore.define_group("TOGO", os.getcwd())
end}})

precore.make_config("togo.test.opt", {
	once = true,
}, {
{option = {
	data = {
		trigger = "togo.test",
		description = "enable test mode",
	},
	init_handler = function()
	end
}}})

precore.make_config("togo.strict", nil, {
{project = function()
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

precore.make_config("togo.base", nil, {
{project = function(p)
	configuration {}
		libdirs {
			S"${TOGO_BUILD}/lib/",
		}

	configuration {"linux"}
		buildoptions {
			"-pthread",
		}

	if not precore.env_project()["NO_LINK"] then
		links {
			"m",
			"dl",
			"pthread",
		}
	end

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

precore.apply_global({
	"precore.env-common",
	"togo.env",
})

togo.libs = {
	"core",
}
togo.tools = {}

local glob = os.matchdirs(G"${TOGO_ROOT}/lib/*")
for _, p in pairs(glob) do
	local name = path.getname(p)
	if name ~= "core" then
		table.insert(togo.libs, name)
	end
end

glob = os.matchdirs(G"${TOGO_ROOT}/tool/*")
for _, p in pairs(glob) do
	local name = path.getname(p)
	table.insert(togo.tools, name)
end

precore.make_config_scoped("togo.projects", {
	once = true,
}, {
{global = function()
	precore.make_solution(
		"togo",
		{"debug", "release"},
		{"x64", "x32"},
		nil,
		{
			"precore.generic",
		}
	)

	local env = {
		NO_LINK = true,
	}
	local configs = {
		"togo.strict",
	}
	for _, name in pairs(togo.libs) do
		table.insert(configs, 1, "togo.lib." .. name .. ".dep")
	end
	for _, name in pairs(togo.tools) do
		table.insert(configs, 1, "togo.tool." .. name .. ".dep")
	end

	precore.make_project(
		"igen",
		"C++", "StaticLib",
		"build/igen/", "build/igen/",
		env, configs
	)

	configuration {"gmake"}
		prebuildcommands {
			"$(SILENT) mkdir -p ./tmp",
			"$(SILENT) ./scripts/collect_igen_users",
			"$(SILENT) ./scripts/run_igen.py -- $(ALL_CXXFLAGS)",
			"$(SILENT) exit 0",
		}
end}})

for _, name in pairs(togo.libs) do
	precore.import(G"${TOGO_ROOT}/lib/" .. name)
end
for _, name in pairs(togo.tools) do
	precore.import(G"${TOGO_ROOT}/tool/" .. name)
end

precore.apply_global({
	"togo.test.opt",
	"togo.lib.game.renderer.opt",
	"togo.lib.game.gfx-backend.opt",
})
