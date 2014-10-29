
#include <togo/assert.hpp>
#include <togo/string.hpp>
#include <togo/filesystem.hpp>
#include <togo/directory_reader.hpp>

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

	#define READ_ENTRY(type_, path_)								\
		TOGO_ASSERTE(												\
			directory_reader::read(reader, entry, type_mask) &&		\
			entry.type == type_ &&									\
			string::compare_equal(entry.path, path_)				\
		)
	{
	DirectoryReader reader{};
	DirectoryEntry entry;
	auto const type_file = DirectoryEntry::Type::file;
	auto const type_dir = DirectoryEntry::Type::dir;
	auto const type_mask = DirectoryEntry::Type::all;

	TOGO_ASSERTE(directory_reader::open(reader, ROOT, true, true));
	READ_ENTRY(type_file, ROOT "/file");
	READ_ENTRY(type_dir , ROOT "/inner_dir/");
	READ_ENTRY(type_file, ROOT "/inner_dir/file");
	TOGO_ASSERTE(!directory_reader::read(reader, entry, type_mask));
	directory_reader::close(reader);
	}

	TOGO_ASSERTE(filesystem::remove_file(ROOT "/.dotdotdot"));
	TOGO_ASSERTE(filesystem::remove_file(ROOT "/file"));
	TOGO_ASSERTE(filesystem::remove_file(ROOT "/inner_dir/file"));
	TOGO_ASSERTE(filesystem::remove_directory(ROOT "/inner_dir"));
	TOGO_ASSERTE(filesystem::remove_directory(ROOT));

	return 0;
}
