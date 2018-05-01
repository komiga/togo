#!/usr/bin/env lua5.1

-- Weee
local SCRIPTS_PATH = string.sub(arg[0], 1, -15)

dofile(SCRIPTS_PATH .. "/common.lua")
require("lfs")

local prev_wd = lfs.currentdir()
lfs.chdir(SCRIPTS_PATH .. "/..")
local ROOT = lfs.currentdir()

local group_data = {}

group_data["core"] = {
	excluded = {
		["algorithm/sort"] = true,
	},
	should_fail = {
		["memory/assert_allocator_f1"] = true,
		["memory/assert_allocator_f2"] = true,
		["memory/fixed_allocator_f1"] = true,
		["memory/fixed_allocator_f2"] = true,
	},
	args = {
		["utility/args"] = '-a --b=1234 --c="goats" cmd -d - -- --- a1 a2 "a3" 1 false true',
		["lua/general"] = 'lua/utility.lua lua/filesystem.lua lua/io.lua',
	},
}

group_data["image"] = {
	excluded = {
	},
	should_fail = {
	},
	args = {
	},
}

group_data["platform"] = {
	excluded = {
	},
	should_fail = {
	},
	args = {
	},
}

group_data["window"] = {
	excluded = {
		-- ["window/window"] = true,
		-- ["window/window_opengl"] = true,
	},
	should_fail = {
	},
	args = {
	},
}

group_data["game"] = {
	excluded = {
		-- ["app/general"] = true,
		-- ["gfx/renderer_triangle"] = true,
		-- ["gfx/renderer_pipeline"] = true,
	},
	should_fail = {
	},
	args = {
	},
}

local group_names = togo_libraries()
local groups = {}

for _, group_name in pairs(group_names) do
	local root = "lib/" .. group_name .. "/test"
	local group = {
		name = group_name,
		root = ROOT .. "/" .. root,
		data = group_data[group_name],
		tests = {},
	}
	for file in iterate_dir(root, "file") do
		local name, ext = split_path(file)
		if ext == "elf" then
			if group.data.excluded[name] then
				printf("EXCLUDED: %s / %s", group_name, name)
			else
				table.insert(group.tests, {name = name, path = file})
			end
		end
	end
	table.insert(groups, group)
end

function run()
	for _, group in pairs(groups) do
		printf("\nGROUP: %s", group.name)
		lfs.chdir(group.root)
		for _, test in pairs(group.tests) do
			local cmd = "./" .. test.path
			local should_fail = group.data.should_fail[test.name]
			local args = group.data.args[test.name]
			if args then
				cmd = cmd .. " " .. args
			end
			printf("\nRUNNING: %s", cmd)
			local exit_code = os.execute(cmd)
			if exit_code ~= 0 then
				if should_fail then
					print("(expected)")
				else
					printf("ERROR: '%s' failed with exit code %d", test.path, exit_code)
					return -1
				end
			end
		end
	end
	return 0
end

local ec = run()
lfs.chdir(prev_wd)
os.exit(ec)
