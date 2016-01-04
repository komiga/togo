
#include <togo/core/error/assert.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/filesystem/filesystem.hpp>
#include <togo/core/filesystem/directory_reader.hpp>

using namespace togo;

signed main() {
	#define ROOT "data/directory_reader"

	#define CREATE_IF_NON_EXISTENT(kind_, path_)					\
		TOGO_ASSERTE(												\
			filesystem::is_ ## kind_ (path_) ||						\
			filesystem::create_ ## kind_ (path_)					\
		)

	CREATE_IF_NON_EXISTENT(directory, ROOT);
	CREATE_IF_NON_EXISTENT(directory, ROOT "/inner_dir");
	CREATE_IF_NON_EXISTENT(file, ROOT "/inner_dir/file");
	CREATE_IF_NON_EXISTENT(file, ROOT "/file");
	CREATE_IF_NON_EXISTENT(file, ROOT "/.dotdotdot");

	{
	DirectoryReader reader{};
	DirectoryEntry entry;
	auto const type_file = DirectoryEntry::Type::file;
	auto const type_dir = DirectoryEntry::Type::dir;
	auto const type_mask = DirectoryEntry::Type::all;

	struct {
		bool found;
		DirectoryEntry::Type type;
		StringRef path;
	}
	*match,
	expected_entries[]{
		{false, type_file, ROOT "/file"},
		{false, type_dir , ROOT "/inner_dir/"},
		{false, type_file, ROOT "/inner_dir/file"}
	};

	TOGO_ASSERTE(directory_reader::open(reader, ROOT, true, true, true));
	while (directory_reader::read(reader, entry, type_mask)) {
		match = nullptr;
		for (auto& x : expected_entries) {
			if (
				entry.type == x.type &&
				string::compare_equal(entry.path, x.path)
			) {
				match = &x;
				break;
			}
		}
		TOGO_ASSERTF(match, "unexpected entry: %.*s", entry.path.size, entry.path.data);
		TOGO_ASSERTE(!match->found);
		match->found = true;
	}
	directory_reader::close(reader);

	for (auto const& x : expected_entries) {
		TOGO_ASSERTF(x.found, "unmatched entry: %.*s", x.path.size, x.path.data);
	}}

	TOGO_ASSERTE(filesystem::remove_file(ROOT "/.dotdotdot"));
	TOGO_ASSERTE(filesystem::remove_file(ROOT "/file"));
	TOGO_ASSERTE(filesystem::remove_file(ROOT "/inner_dir/file"));
	TOGO_ASSERTE(filesystem::remove_directory(ROOT "/inner_dir"));
	TOGO_ASSERTE(filesystem::remove_directory(ROOT));

	return 0;
}
