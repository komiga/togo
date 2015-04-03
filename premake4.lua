
dofile("precore_import.lua")

local _, G, R = precore.helpers()

precore.init(
	nil,
	{
		"precore.clang-opts",
		"precore.c++11-core",
	}
)

precore.import(".")
precore.apply_global("togo.projects")

-- precore.print_debug()

if _ACTION == "clean" then
	os.rmdir(G"${BUILD_PATH}")
end
