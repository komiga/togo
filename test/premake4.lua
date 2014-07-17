
dofile("../premake_common.lua")

function make_test(group, name, srcglob, configs)
	precore.make_project(
		group .. "_" .. name,
		"C++", "ConsoleApp",
		"./", "out/",
		nil, configs
	)
	precore.apply("togo-strict")
	precore.apply("threading-support")

	if nil == srcglob then
		srcglob = name .. ".cpp"
	end

	configuration {}
		targetname(name)
		files {
			srcglob
		}

	configuration {"linux"}
		targetsuffix(".elf")
end

function make_tests(group, tests)
	for name, test in pairs(tests) do
		make_test(group, name, test[1], test[2])
	end
end

precore.make_config(
"threading-support", {{
project = function()
	configuration {"linux"}
		buildoptions {
			"-pthread"
		}
		links {"pthread"}
end}})

-- Test solution

precore.make_solution(
	"test",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic",
		"togo-deps",
		"togo-import"
	}
)

-- Groups

include("general")
include("collections")
include("memory")
include("hash")
include("threading")
include("system")
include("io")
include("algorithm")
include("utility")

action_clean()
