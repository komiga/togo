
dofile("../premake_common.lua")

function make_test(group, name, srcglob, configs)
	precore.make_project(
		group .. "_" .. name,
		"C++", "ConsoleApp",
		"./", "out/",
		nil, configs
	)
	precore.apply("togo-strict")

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

-- Test solution

precore.make_solution(
	"test",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic",
		"togo-deps",
		"togo-import",
		"togo-deps-link"
	}
)

-- Groups

include("algorithm")
include("app")
include("collection")
include("filesystem")
include("general")
include("gfx")
include("hash")
include("io")
include("kvs")
include("memory")
include("random")
include("resource")
include("serialization")
include("string")
include("system")
include("threading")
include("utility")

precore.action_clean("out")
