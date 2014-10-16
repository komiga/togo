
dofile("premake_common.lua")

function make_tool(name, configs)
	precore.make_project(
		"tool_" .. name,
		"C++", "ConsoleApp",
		"bin/", "out/tool_" .. name .. "/",
		nil, configs
	)
	precore.apply("togo-deps")
	precore.apply("togo-import")
	precore.apply("togo-deps-link")

	configuration {}
		targetname(name)
		includedirs {
			"src/"
		}
		files {
			"src/togo/tool_" .. name .. "/**.cpp"
		}

	configuration {"linux"}
		targetsuffix(".elf")
end

-- Core solution

precore.make_solution(
	"togo",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic",
		"togo-strict",
		"togo-deps",
		"togo-config",
	}
)

-- Core library

precore.make_project(
	"togo",
	"C++", "StaticLib",
	"lib/", "out/togo/",
	nil, nil
)

configuration {"debug"}
	targetsuffix("_d")

configuration {}
	includedirs {
		"src/"
	}
	files {
		"src/togo/**.cpp"
	}
	excludes {
		"src/togo/tool_**"
	}

-- Tools

make_tool("build", nil)

action_clean()
