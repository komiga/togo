
package.path = package.path .. ';' .. [[dep/?.lua]]

local dkjson = require("dep/dkjson/dkjson")
dofile("scripts/common.lua")

local THRESHOLD = 150

local MATCHERS = {
	['@defgroup%s+(.+)%s+.+$'] = function(ctx, group, data, group_name)
		assert(not data.doc_group)
		data.doc_group = group_name
	end,
	['@ingroup%s+(.+)$'] = function(ctx, group, data, group_name)
		table.insert(data.ingroups, group_name)
	end,
	['#include.+[<"](.+)[>"]'] = function(ctx, group, data, path)
		local _, extension = split_path(path)
		if extension == "gen_interface" then
			assert(data.gen_path == nil, "a gen_interface was already included!")
			data.gen_path = group.src .. '/' .. path
			return true
		end
	end,
	['igen%-following%-sources%-included$'] = function(ctx, group, data, _)
		data.sources_included = true
	end,
	['igen%-source:%s*([^%s]+)$'] = function(ctx, group, data, path)
		table.insert(data.sources, {
			path = group.src_inner .. '/' .. path,
			included = data.sources_included,
		})
	end,
	['igen%-source%-pattern:%s*([^%s]+)$'] = function(ctx, group, data, pattern)
		pattern = string.format("^%s/%s$", group.src_inner, pattern)
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

function process_file(ctx, group, path)
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
				continue = func(ctx, group, data, m) ~= true
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

function collect_groups(ctx, path, src_inner_prefix)
	for name, _ in iterate_dir(path, "directory", 1) do
		local src = path .. "/" .. name .. "/src"
		local group = {
			name = name,
			src = src,
			src_inner = src .. "/togo/" .. src_inner_prefix .. name,
			user_paths = {},
		}
		table.insert(ctx.groups, group)
	end
end

function main(arguments)
	local ctx = {
		groups = {},
		path_exists = {},
		paths = {},
		user_paths = {},
		users = {},
	}

	local ext_filter = make_inverse_table({
		"hpp",
	}, true)

	collect_groups(ctx, "lib", "")
	collect_groups(ctx, "tool", "tool_")
	for _, group in pairs(ctx.groups) do
		for rel_path, _ in iterate_dir(group.src, "file") do
			local path = group.src .. '/' .. rel_path
			local _, extension = split_path(rel_path)
			ctx.path_exists[path] = true
			table.insert(ctx.paths, path)
			if ext_filter[extension] then
				assert(not ctx.user_paths[path], "duplicate path: " .. group.src .. ", " .. path)
				table.insert(group.user_paths, path)
			end
		end
		table.sort(group.user_paths)
	end
	table.sort(ctx.paths)

	for _, group in pairs(ctx.groups) do
		for _, path in pairs(group.user_paths) do
			process_file(ctx, group, path)
		end
	end
	write_users(ctx)
	return 0
end

os.exit(main(arg))
