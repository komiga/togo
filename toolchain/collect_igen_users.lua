
package.path = package.path .. ';' .. [[dep/?.lua]]

require("lfs")
local dkjson = require("dep/dkjson/dkjson")

local THRESHOLD = 150

function table_last(t)
	return t[#t]
end

function split_path(path)
	return string.match(path, "^(%.?%.?[^%.]*)%.?([^%.\\/]*)$")
end

function trim_trailing_slash(path)
	if string.sub(path, -1) == "/" then
		path = string.sub(path, 1, -2)
	end
	return path
end

function make_inverse_table(t, value)
	local it = {}
	for k, v in pairs(t) do
		it[v] = value or k
	end
	return it
end

function iterate_dir(dir, select_only, max_depth)
	assert(dir and dir ~= "", "directory parameter is missing or empty")
	dir = trim_trailing_slash(dir)

	local function yield_tree(base, path, depth)
		if max_depth ~= nil and depth > max_depth then
			return
		end
		for entry in lfs.dir(base .. path) do
			if entry ~= "." and entry ~= ".." then
				entry = path .. "/" .. entry
				local attr = lfs.attributes(base .. entry)
				if select_only == nil or attr.mode == select_only then
					coroutine.yield(string.sub(entry, 2), attr)
				end
				if attr.mode == "directory" then
					yield_tree(base, entry, depth + 1)
				end
			end
		end
	end

	return coroutine.wrap(
		function()
			yield_tree(dir, "", 1)
		end
	)
end

local MATCHERS = {
	['@defgroup%s+(.+)$'] = function(ctx, lib, data, group_name)
		assert(not data.doc_group)
		data.doc_group = group_name
	end,
	['@ingroup%s+(.+)$'] = function(ctx, lib, data, group_name)
		table.insert(data.ingroups, group_name)
	end,
	['#include.+[<"](.+)[>"]'] = function(ctx, lib, data, path)
		local _, extension = split_path(path)
		if extension == "gen_interface" then
			assert(data.gen_path == nil, "a gen_interface was already included!")
			-- data.gen_path = lib.src .. '/' .. path:gsub("^togo/" .. lib.name .. "/", "")
			data.gen_path = lib.src .. '/' .. path
			return true
		end
	end,
	['igen%-following%-sources%-included$'] = function(ctx, lib, data, _)
		data.sources_included = true
	end,
	['igen%-source:%s*([^%s]+)$'] = function(ctx, lib, data, path)
		table.insert(data.sources, {
			path = lib.src_inner .. '/' .. path,
			included = data.sources_included,
		})
	end,
	['igen%-source%-pattern:%s*([^%s]+)$'] = function(ctx, lib, data, pattern)
		pattern = string.format("^%s/%s$", lib.src_inner, pattern)
		for _, path in pairs(ctx.paths) do
			local i, _ = string.find(path, pattern)
			if i ~= nil then
				table.insert(data.sources, {
					path = path,
					included = data.sources_included,
				})
			end
		end
	end,
}

function process_file(ctx, lib, path)
	local stream, err = io.open(path, "r")
	if stream == nil then
		error("failed to open '" .. path .. "': " .. err)
	end
	local path_no_ext, _ = split_path(path)
	local data = {
		slug = path_no_ext,
		path = path,
		gen_path = nil,
		sources_included = false,
		sources = {},
		doc_group = nil,
		ingroups = {},
	}

	local primary_source = path_no_ext .. ".cpp"
	if ctx.path_exists[primary_source] ~= nil then
		table.insert(data.sources, {
			path = primary_source,
			included = data.sources_included,
		})
	end

	local continue = true
	local line
	local line_position = 1
	while continue do
		line = stream:read("*l")
		if line == nil or THRESHOLD < line_position then
			break
		end

		for pattern, func in pairs(MATCHERS) do
			local m = string.match(line, pattern)
			if m ~= nil then
				continue = func(ctx, lib, data, m) ~= true
				break
			end
		end
		line_position = line_position + 1
	end
	stream:close()

	if data.gen_path == nil then
		return false
	end

	if not data.doc_group then
		data.doc_group = #data.ingroups > 0 and table_last(data.ingroups) or nil
		if data.doc_group then
			print(data.path ..  " => " .. data.doc_group)
		end
	end
	data.ingroups = nil
	data.sources_included = nil
	table.insert(ctx.users, data)
	return true
end

local USERS_PATH = "tmp/igen_users"

function write_users(ctx)
	local path = USERS_PATH
	local stream, err = io.open(path, "w")
	if stream == nil then
		error("failed to open '" .. path .. "': " .. err)
	end
	stream:write(dkjson.encode({users = ctx.users}, {indent = false}))
	stream:close()
end

function main(arguments)
	local ctx = {
		libraries = {},
		path_exists = {},
		paths = {},
		user_paths = {},
		users = {},
	}

	local ext_filter = make_inverse_table({
		"hpp",
	}, true)

	for lib_name, _ in iterate_dir("lib", "directory", 1) do
		local dir = "lib/" .. lib_name .. "/src"
		local lib = {
			name = lib_name,
			src = dir,
			src_inner = dir .. "/togo/" .. lib_name,
			user_paths = {},
		}
		for rel_path, _ in iterate_dir(dir, "file") do
			local path = dir .. '/' .. rel_path
			local _, extension = split_path(rel_path)
			ctx.path_exists[path] = true
			table.insert(ctx.paths, path)
			if ext_filter[extension] then
				assert(not ctx.user_paths[path], "duplicate path: " .. dir .. ", " .. path)
				table.insert(lib.user_paths, path)
			end
		end
		table.sort(lib.user_paths)
		table.insert(ctx.libraries, lib)
	end
	table.sort(ctx.paths)

	for _, lib in pairs(ctx.libraries) do
		for _, path in pairs(lib.user_paths) do
			process_file(ctx, lib, path)
		end
	end
	write_users(ctx)
	return 0
end

os.exit(main(arg))
