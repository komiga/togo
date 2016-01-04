
local U = require "togo.utility"
local FS = require "togo.filesystem"

U.print("%d", FS.EntryType.file)
U.print("%d", FS.EntryType.dir)
U.print("%d", FS.EntryType.all)

local entry_type_name = {
	[FS.EntryType.file] = "file",
	[FS.EntryType.dir] = "dir",
	[FS.EntryType.all] = "all",
}

for path, entry_type in FS.iterate_dir(".", FS.EntryType.all, true, true) do
	U.print("%-4s %s", entry_type_name[entry_type], path)
end
