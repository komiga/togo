
require "lfs"

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
	['igen%-source:%s*([^%s]+)$'] = function(ctx, data, path)
		table.insert(data.sources, path)
	end,
	['igen%-source%-pattern:%s*([^%s]+)$'] = function(ctx, data, pattern)
		pattern = string.format("^%s$", pattern)
		for _, path in pairs(ctx.paths) do
			local i, _ = string.find(path, pattern)
			if i ~= nil then
				table.insert(data.sources, path)
			end
		end
	end,
}

function process_file(ctx, from, path)
	local stream, err = io.open(path, "r")
	if stream == nil then
		error("failed to open '" .. path .. "': " .. err)
	end
	local slug = path:gsub("^" .. from .. "/", ""):gsub("%.hpp$", "")
	local data = {
		from = from,
		slug = slug,
		header = path,
		gen_path = nil,
		sources = {},
		ingroups = {},
	}

	local primary_source = path:gsub("hpp$", "cpp")
	if ctx.path_from[primary_source] ~= nil then
		table.insert(data.sources, primary_source)
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

	data.doc_group = #data.ingroups > 0 and table_last(data.ingroups) or nil
	table.insert(ctx.users, data)
	return true
end

local USERS_PATH = "toolchain/igen_users"

local FIELDS = {
	"slug",
	"header",
	"sources",
	"gen_path",
	"doc_group",
}

local FIELD_FMT = '\t"%s" : %s'

function json_array(t)
	local str = "[\n"
	for i, v in ipairs(t) do
		str = str .. string.format('\t\t"%s"', tostring(v))
		if i < #t then
			str = str .. ",\n"
		end
	end
	return str .. "\n\t]"
end

function write_users(ctx)
	local path = USERS_PATH
	local stream, err = io.open(path, "w")
	if stream == nil then
		error("failed to open '" .. path .. "': " .. err)
	end
	stream:write('{"users" : [\n')
	for i, user in ipairs(ctx.users) do
		stream:write("{\n")
		for j, name in pairs(FIELDS) do
			local field = user[name]
			if type(field) == "table" then
				field = json_array(field)
			else
				field = string.format('"%s"', tostring(field))
			end
			stream:write(string.format(FIELD_FMT, name, field))
			stream:write(j < #FIELDS and ",\n" or "\n")
		end
		stream:write("}")
		if i < #ctx.users then
			stream:write(",")
		end
	end
	stream:write("]}\n")
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

	for i = 1, #arguments do
		local dir = trim_trailing_slash(arguments[i])
		for rel_path, _ in iterate_dir(dir, "file") do
			local path = dir .. '/' .. rel_path
			local _, extension = split_path(rel_path)
			ctx.path_from[path] = dir
			table.insert(ctx.paths, path)
			if extension == "hpp" then
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
