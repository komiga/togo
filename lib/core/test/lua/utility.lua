
function print_preload()
	print("preload:")
	for k, v in pairs(package.preload) do
		print("  " .. k .. " = " .. type(v))
	end
end

print_preload()

local U = require "togo.utility"

U.log("util test")

print_preload()

do
	local t = {
		{"/x/", "x"},
		{"/x", "x"},
		{"x", "x"},
	}
	for _, p in pairs(t) do
		local r = U.trim_slashes(p[1])
		U.print("%s -> %s", p[1], r)
		U.assert(r == p[2])
	end
end

do
	local command_parsed, opt, cmd_opt, cmd = U.parse_args({
		"exec",
		"-a",
		"--b=c",

		"x",
		"-y",
		"1",
		"2",
		"-z",
		"3",
	})
	U.assert(command_parsed)

	local t, i
	local function check(name, value)
		local p = t[i]
		U.assert(p.name == name and p.value == value)
		i = i + 1
	end

	t = opt; i = 1
	U.assert(t.name == "exec")
	check("-a", true)
	check("--b", "c")

	t = cmd_opt; i = 1
	U.assert(t.name == "")
	check("-y", true)

	t = cmd; i = 1
	U.assert(t.name == "x")
	check("", "1")
	check("", "2")
	check("-z", true)
	check("", "3")
end
