
function table_last(t)
	return t[#t]
end

function split_path(path)
	local name, extension = string.match(path, "^(%.?%.?.*)%.([^%.\\/]*)$")
	return (name or path), extension
end

function dirname(path)
	path = trim_trailing_slash(path)
	local dir, _ = string.match(path, "^(.+)/([^/]*)$")
	return dir or path
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

function printf(msg, ...)
	print(string.format(msg, ...))
end

function iterate_dir(dir, select_only, max_depth)
	require("lfs")
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

function togo_libraries()
	return {
		"core",
		"image",
		"window",
		"game",
	}
end

function togo_tools()
	return {
		"res_build",
	}
end
