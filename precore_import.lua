
local function import_precore()
	local root_path = os.getenv("PRECORE_ROOT")
	if root_path == nil or #root_path == 0 then
		error(
			"Environment variable 'PRECORE_ROOT' is not defined" ..
			" or is blank; assign it to precore's root directory"
		)
	end
	dofile(path.join(root_path, "precore.lua"))
end

import_precore()
