
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

function iterate_dir(dir, select_only)
	assert(dir and dir ~= "", "directory parameter is missing or empty")
	dir = trim_trailing_slash(dir)

	local function yield_tree(base, path)
		for entry in lfs.dir(base .. path) do
			if entry ~= "." and entry ~= ".." then
				entry = path .. "/" .. entry
				local attr = lfs.attributes(base .. entry)
				if select_only == nil or attr.mode == select_only then
					coroutine.yield(string.sub(entry, 2), attr)
				end
				if attr.mode == "directory" then
					yield_tree(base, entry)
				end
			end
		end
	end

	return coroutine.wrap(
		function()
			yield_tree(dir, "")
		end
	)
end

local MATCHERS = {
	['@ingroup%s+(.+)$'] = function(ctx, data, group_name)
		table.insert(data.ingroups, group_name)
	end,
	['#include.+[<"](.+)[>"]'] = function(ctx, data, include_path)
		local _, extension = split_path(include_path)
		if extension == "gen_interface" then
			assert(data.gen_path == nil, "a gen_interface was already included!")
			data.gen_path = "src/" .. include_path
			return true
		end
	end,
	['igen%-following%-sources%-included$'] = function(ctx, data, _)
		data.sources_included = true
	end,
	['igen%-source:%s*([^%s]+)$'] = function(ctx, data, path)
		table.insert(data.sources, {
			path = path,
			included = data.sources_included,
		})
	end,
	['igen%-source%-pattern:%s*([^%s]+)$'] = function(ctx, data, pattern)
		pattern = string.format("^%s$", pattern)
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

function process_file(ctx, from, path)
	local stream, err = io.open(path, "r")
	if stream == nil then
		error("failed to open '" .. path .. "': " .. err)
	end
	local path_no_ext = path:gsub("%.[^%.]+$", "")
	local data = {
		from = from,
		slug = path_no_ext:gsub("^" .. from .. "/", ""),
		path = path,
		gen_path = nil,
		sources_included = false,
		sources = {},
		ingroups = {},
	}

	local primary_source = path_no_ext .. ".cpp"
	if ctx.path_from[primary_source] ~= nil then
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
				continue = func(ctx, data, m) ~= true
				break
			end
		end
		line_position = line_position + 1
	end
	stream:close()

	if data.gen_path == nil then
		return false
	end

	data.from = nil
	data.doc_group = #data.ingroups > 0 and table_last(data.ingroups) or nil
	data.ingroups = nil
	data.sources_included = nil
	table.insert(ctx.users, data)
	return true
end

local USERS_PATH = "toolchain/igen_users"

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
	if #arguments == 0 then
		print("usage: collect_igen_users dir [dir ...]")
		return 0
	end

	local ctx = {
		path_from = {},
		paths = {},
		user_paths = {},
		users = {},
	}

	local ext_filter = make_inverse_table({
		"hpp",
	}, true)

	for i = 1, #arguments do
		local dir = trim_trailing_slash(arguments[i])
		for rel_path, _ in iterate_dir(dir, "file") do
			local path = dir .. '/' .. rel_path
			local _, extension = split_path(rel_path)
			ctx.path_from[path] = dir
			table.insert(ctx.paths, path)
			if ext_filter[extension] then
				assert(not ctx.user_paths[path], "duplicate path: " .. dir .. ", " .. path)
				table.insert(ctx.user_paths, path)
			end
		end
	end
	table.sort(ctx.paths)
	table.sort(ctx.user_paths)

	for _, path in pairs(ctx.user_paths) do
		process_file(ctx, ctx.path_from[path], path)
	end
	write_users(ctx)
	return 0
end

os.exit(main(arg))
