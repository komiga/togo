
dofile("premake_common.lua")

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
		"togo-config"
	}
)

-- Core library

precore.make_project(
	"togo",
	"C++", "StaticLib",
	"lib/", "out/",
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

action_clean()
