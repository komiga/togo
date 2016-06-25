
local S, G, R = precore.helpers()

local configs = {
	"togo.lib.core.dep",
}

togo.make_tests("algorithm", {
	["sort"] = {nil, configs},
})

togo.make_tests("collection", {
	["array"] = {nil, configs},
	["fixed_array"] = {nil, configs},
	["hash_map"] = {nil, configs},
	["npod"] = {nil, configs},
	["priority_queue"] = {nil, configs},
	["queue"] = {nil, configs},
})

togo.make_tests("filesystem", {
	["general"] = {nil, configs},
	["directory_reader"] = {nil, configs},
})

togo.make_tests("general", {
	["headers"] = {nil, configs},
})

togo.make_tests("hash", {
	["literal"] = {nil, configs},
})

togo.make_tests("io", {
	["file_stream"] = {nil, configs},
	["memory_stream"] = {nil, configs},
	["object_buffer"] = {nil, configs},
})

togo.make_tests("kvs", {
	["general"] = {nil, configs},
	["io"] = {nil, configs},
	["echo"] = {nil, configs},
})

togo.make_tests("memory", {
	["init"] = {nil, configs},
	["assert_allocator_f1"] = {nil, configs},
	["assert_allocator_f2"] = {nil, configs},
	["fixed_allocator"] = {nil, configs},
	["fixed_allocator_f1"] = {nil, configs},
	["fixed_allocator_f2"] = {nil, configs},
	["temp_allocator"] = {nil, configs},
})

togo.make_tests("random", {
	["rng"] = {nil, configs},
})

togo.make_tests("serialization", {
	["general"] = {nil, configs},
})

togo.make_tests("string", {
	["general"] = {nil, configs},
})

togo.make_tests("system", {
	["general"] = {nil, configs},
})

togo.make_tests("threading", {
	["thread"] = {nil, configs},
	["mutex"] = {nil, configs},
	["condvar"] = {nil, configs},
	["task_manager"] = {nil, configs},
})

togo.make_tests("utility", {
	["general"] = {nil, configs},
	["endian"] = {nil, configs},
	["args"] = {nil, configs},
})

togo.make_tests("parser", {
	["general"] = {nil, configs},
})

togo.make_tests("lua", {
	["general"] = {nil, configs},
})
