
require "lfs"

THRESHOLD = 150

function table_last(t)
	return t[#t]
end

function split_path(path)
	return string.match(path, "^(%.?%.?[^%.]*)%.?([^%.\\/]*)$")
end

function parse(stream, slug, threshold)
	assert(threshold > 0)
	local data = {
		slug = slug,
		header = slug .. ".hpp",
		source = slug .. ".cpp",
		gen_path = nil,
		ingroups = {},
	}

	local line = ""
	local line_position = 1
	local group_name
	local include_path, extension
	while true do
		line = stream:read("*l")
		if line == nil or threshold < line_position then
			break
		end

		group_name = string.match(line, "@ingroup%s+(.+)$")
		if group_name ~= nil then
			table.insert(data.ingroups, group_name)
		else
			include_path = string.match(line, "#include.+[<\"](.+)[>\"]")
			if include_path ~= nil then
				_, extension = split_path(include_path)
				if extension == "gen_interface" then
					assert(data.gen_path == nil, "a gen_interface was already included!")
					data.gen_path = include_path:gsub("^togo/", "")
					break
				end
			end
		end
		line_position = line_position + 1
	end

	data.doc_group = #data.ingroups > 0 and table_last(data.ingroups) or nil
	data.namespace = string.match(data.slug, "/(.+)$")
	return data
end

function process_file(users, path)
	local stream, err = io.open(path, "r")
	if stream == nil then
		error("failed to open '" .. path .. "': " .. err)
	end
	local slug = path:gsub("^src/togo/", ""):gsub("%.hpp$", "")
	local data = parse(stream, slug, THRESHOLD)
	stream:close()
	if data.gen_path ~= nil then
		table.insert(users, data)
		return true
	end
	return false
end

USERS_PATH = "toolchain/igen_users"

FIELDS = {
	"slug",
	"header",
	"source",
	"gen_path",
	"namespace",
	"doc_group",
}

FIELD_FMT = [[	"%s" : "%s"]]

function write_users(users)
	local path = USERS_PATH
	local stream, err = io.open(path, "w")
	if stream == nil then
		error("failed to open '" .. path .. "': " .. err)
	end
	stream:write('{"users" : [\n')
	local user
	for i = 1, #users do
		user = users[i]
		stream:write("{\n")
		for j, name in pairs(FIELDS) do
			stream:write(string.format(FIELD_FMT, name, user[name]))
			stream:write(j < #FIELDS and ",\n" or "\n")
		end
		stream:write("}")
		if i < #users then
			stream:write(",")
		end
	end
	stream:write("]}\n")
	stream:close()
end

function main(arguments)
	if #arguments == 0 then
		print("usage: collect_igen_users file [file ...]")
		return 0
	end

	local paths = {}
	for i = 1, #arguments do
		table.insert(paths, arguments[i])
	end
	local users = {}
	for _, path in pairs(paths) do
		process_file(users, path)
	end
	write_users(users)
	return 0
end

os.exit(main(arg))
