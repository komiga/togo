u8R""__RAW_STRING__(

togo = togo or {}
togo.utility = togo.utility or {}
local M = togo.utility

M.debug = false

function M.ternary(cond, a, b)
	return cond and a or b
end

function M.optional(value, default)
	if value ~= nil then
		return value
	end
	return default
end

function M.optional_in(table, name, default)
	M.type_assert(table, "table")
	M.type_assert(name, "string")
	table[name] = M.optional(table[name], default)
	return table[name]
end

function M.type_class(x)
	local t = type(x)
	if t == "table" then
		local mt = getmetatable(x)
		if mt then
			return mt
		end
	end
	return t
end

function M.is_type(x, t, basic)
	local xt = basic and type(x) or M.type_class(x)
	if type(t) == "function" then
		return t(x, xt)
	end
	return t == xt
end

function M.is_type_any(x, types, opt)
	M.type_assert(types, "table")

	if opt and x == nil then
		return true
	end
	local xt = M.type_class(x)
	for _, t in pairs(types) do
		if type(t) == "function" then
			if t(x, xt) then
				return true
			end
		elseif t == xt then
			return true
		end
	end
	return false
end

function M.T_instance(c)
	return function(x, t)
		return c == t
	end
end

function M.is_class(x)
	if type(x) == "table" then
		local mt = getmetatable(x)
		return (
			x == rawget(x, "__index") and
			mt == rawget(x, "__class_static")
		)
	end
	return false
end

function M.is_instance(x, of)
	if type(x) == "table" then
		local mt = getmetatable(x)
		if mt then
			if of then
				return mt == of
			else
				return mt == rawget(mt, "__index")
			end
		end
	end
	return false
end

function M.get_trace(level)
	local info = debug.getinfo(level + 2, "Sl")
	-- return M.pad_left(info.short_src, 28) .. " @ " .. M.pad_right(tostring(info.currentline), 4)
	return info.short_src .. " @ " .. M.pad_right(tostring(info.currentline), 4)
end

function M.assertl(level, e, msg, ...)
	if not e then
		error(M.get_trace(level + 1) .. ": assertion failed: " .. string.format(msg or "<expression>", ...), 0)
	end
end

function M.assert(e, msg, ...)
	M.assertl(1, e, msg, ...)
end

function M.type_assert(x, t, opt, level)
	level = M.optional(level, 0)
	M.assertl(
		level + 1,
		(opt and x == nil) or M.is_type(x, t, false),
		"utility.type_assert: '%s' (a %s) is not of type %s",
		tostring(x), tostring(type(x)), tostring(t)
	)
	return x
end

function M.type_assert_any(x, types, opt, level)
	M.type_assert(types, "table")
	level = M.optional(level, 0)

	if opt and x == nil then
		return x
	end
	if M.is_type_any(x, types) then
		return x
	end
	M.assertl(
		level + 1, false,
		"utility.type_assert_any: '%s' (a %s) is not of any types specified",
		tostring(x), tostring(type(x))
	)
	return x
end

function M.table_last(table, allow_empty)
	M.assert(#table > 0 or allow_empty)
	return table[#table]
end

function M.table_add(a, b)
	M.type_assert(a, "table")
	M.type_assert(b, "table")

	for k, v in pairs(b) do
		a[k] = v
	end
end

function M.table_ijoined(...)
	local r = {}
	for _, t in ipairs({...}) do
		M.type_assert(t, "table")
		for _, v in ipairs(t) do
			table.insert(r, v)
		end
	end
	return r
end

function M.table_inverse(t, value)
	local it = {}
	for k, v in pairs(t) do
		it[v] = value or k
	end
	return it
end

function M.table_keys(t)
	local kt = {}
	for k, _ in pairs(t) do
		table.insert(kt, k)
	end
	return kt
end

function M.table_copy(dst, src, instances)
	for k, v in pairs(src) do
		if type(v) == "table" then
			if M.is_instance(v) then
				if instances then
					v = v:copy()
				end
			else
				v = M.table_copy({}, v, instances)
			end
		end
		rawset(dst, k, v)
	end
	return dst
end

function M.table_copy_shallow(dst, src, instances)
	for k, v in pairs(src) do
		if M.is_instance(v) then
			if instances then
				v = v:copy()
			end
		end
		rawset(dst, k, v)
	end
	return dst
end

function M.pad_left(str, length)
	return string.rep(' ', M.max(0, length - #str)) .. str
end

function M.pad_right(str, length)
	return str .. string.rep(' ', M.max(0, length - #str))
end

function M.trace()
	print(M.get_trace(1) .. ": TRACE")
end

function M.print(msg, ...)
	M.type_assert(msg, "string")
	print(string.format(msg, ...))
end

function M.logl(level, msg, ...)
	M.type_assert(msg, "string")
	print(M.get_trace(level + 1) .. ": " .. string.format(msg, ...))
end

function M.log(...)
	M.logl(1, ...)
end

function M.args_list(...)
	local t = {...}
	if type(t[1]) == "table" then
		t = t[1]
	end
	return t
end

function M.args(...)
	return unpack(M.args_list(...))
end

function M.errorl(level, msg, ...)
	error(M.get_trace(level + 1) .. ": error: " .. string.format(msg, ...), 0)
end

function M.error(...)
	M.errorl(1, ...)
end

function M.log_debug_closure(m)
	return function(msg, ...)
		M.type_assert(msg, "string")
		if m.debug then
			print(M.get_trace(1) .. ": debug: " .. string.format(msg, ...))
		end
	end
end

M.log_debug = M.log_debug_closure(M)

function M.min(x, y)
	return x < y and x or y
end

function M.max(x, y)
	return x > y and x or y
end

function M.clamp(x, min, max)
	return x < min and min or x > max and max or x
end

local BYTE_FSLASH = string.byte('/', 1)
local BYTE_BSLASH = string.byte('\\', 1)
local BYTE_DOT = string.byte('.', 1)

function M.trim_leading_slashes(s)
	local b
	for i = 1, #s do
		b = string.byte(s, i)
		if b ~= BYTE_FSLASH and b ~= BYTE_BSLASH then
			return string.sub(s, i, -1)
		end
	end
	return s
end

function M.trim_trailing_slashes(s)
	local b
	for i = #s, 1, -1 do
		b = string.byte(s, i)
		if b ~= BYTE_FSLASH and b ~= BYTE_BSLASH then
			return string.sub(s, 1, i)
		end
	end
	return s
end

function M.trim_slashes(s)
	return M.trim_leading_slashes(M.trim_trailing_slashes(s))
end

-- a/b/c -> a
-- a -> nil
function M.rootname(s)
	M.type_assert(s, "string")
	for i = 1, #s do
		if string.byte(s, i) == BYTE_FSLASH then
			return string.sub(s, 1, i - 1)
		end
	end
	return nil
end

function M.strip_extension(s)
	M.type_assert(s, "string")
	local b = nil
	for i = #s, 1, -1 do
		b = string.byte(s, i)
		if b == BYTE_FSLASH then
			break
		elseif b == BYTE_DOT then
			return string.sub(s, 1, i - 1)
		end
	end
	return s
end

function M.join_paths(...)
	local parts = {...}
	local path = ""
	for i, p in ipairs(parts) do
		M.type_assert(p, "string")
		if p ~= "" then
			path = path .. p
			local tail = string.byte(p, #p)
			if i ~= #parts and not (tail == BYTE_FSLASH or tail == BYTE_BSLASH) then
				path = path .. "/"
			end
		end
	end
	return path
end

local function env_upvalue_index(f)
	local index = 1
	repeat
		local name, value = debug.getupvalue(f, index)
		if name == "_ENV" then
			return index, value
		end
		index = index + 1
	until name == nil
	return nil
end

function M.get_env(f)
	return select(2, env_upvalue_index(f))
end

function M.set_env(f, env)
	local index, _ = env_upvalue_index(f)
	if index ~= nil then
		debug.setupvalue(f, index, env)
	end
end

function M.set_functable(t, func)
	if not t.__class_static then
		t.__class_static = {}
		t.__class_static.__index = t.__class_static
		setmetatable(t, t.__class_static)
	end
	t.__class_static.__call = func
end

function M.is_functable(x)
	local mt = getmetatable(x)
	return mt and mt.__call ~= nil
end

function M.class(c)
	if c == nil then
		c = {}
	end
	c.__index = c
	M.set_functable(c, function(c, ...)
		local obj = {}
		setmetatable(obj, c)
		obj:__init(...)
		return obj
	end)
	return c
end

function M.make_empty_object(c)
	M.assert(c ~= nil and type(c) == "table")
	local obj = {}
	setmetatable(obj, c)
	return obj
end

function M.module(name)
	M.type_assert(name, "string")

	local m = _G
	for p in string.gmatch(name, "([^.]+)%.?") do
		if not m[p] then
			m[p] = {}
		end
		m = m[p]
	end
	m._NAME = name
	m._M = m
	return m
end

assert(M == M.module("togo.utility"))
return M

)"__RAW_STRING__"