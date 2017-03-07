
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/filesystem/filesystem.hpp>

using namespace togo;

signed main() {
	StringRef const exec_dir = filesystem::exec_dir();
	TOGO_LOGF("exec_dir: '%.*s'\n", exec_dir.size, exec_dir.data);

	// Working directory
	FixedArray<char, 256> wd_init{};
	unsigned const wd_init_size = filesystem::working_dir(wd_init);
	TOGO_LOGF("wd_init: '%.*s'\n", string::size(wd_init), fixed_array::begin(wd_init));
	TOGO_ASSERTE(wd_init_size > 0 && wd_init_size == fixed_array::size(wd_init));

	// Change directory
	TOGO_ASSERTE(filesystem::set_working_dir("/"));
	FixedArray<char, 256> wd_root{};
	unsigned const wd_root_size = filesystem::working_dir(wd_root);
	TOGO_LOGF("wd_root: '%.*s'\n", string::size(wd_root), fixed_array::begin(wd_root));
	TOGO_ASSERTE(wd_root_size > 0 && wd_root_size == fixed_array::size(wd_root));
	TOGO_ASSERTE(string::compare_equal(wd_root, "/"));

	// Move to group dir
	TOGO_ASSERTE(filesystem::set_working_dir(exec_dir));

	// Invariants based on test & filesystem
	TOGO_ASSERTE(!filesystem::is_file("/non/existent/file"));
	TOGO_ASSERTE(!filesystem::is_file("."));
	#if defined(TOGO_PLATFORM_LINUX)
		#define TEST_EXEC_FILE "general.elf"
	#elif defined(TOGO_PLATFORM_WINDOWS)
		#define TEST_EXEC_FILE "general.exe"
	#endif
	TOGO_ASSERTE(filesystem::is_file(TEST_EXEC_FILE));

	TOGO_ASSERTE(!filesystem::is_directory("/non/existent/directory"));
	TOGO_ASSERTE( filesystem::is_directory("."));
	TOGO_ASSERTE( filesystem::is_directory(".."));

	#define TEST_DIR "test_dir"
	#define TEST_DIR_WHOLE "test_dir_whole"
	#define TEST_FILE TEST_DIR "/test_file"
	#define TEST_FILE_MOVED TEST_DIR "/test_file_moved"
	#define TEST_FILE_COPIED TEST_DIR "/test_file_copied"

	// Directory creation
	TOGO_ASSERTE(!filesystem::is_directory(TEST_DIR));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR, true));

	TOGO_ASSERTE(filesystem::create_directory(TEST_DIR));
	TOGO_ASSERTE(filesystem::is_directory(TEST_DIR));
	TOGO_ASSERTE(!filesystem::create_directory(TEST_DIR));
	TOGO_ASSERTE(filesystem::create_directory(TEST_DIR, true));
	TOGO_ASSERTE(
		filesystem::time_last_modified(TEST_DIR) >
		system::secs_since_epoch() - 2
	);

	// File creation
	TOGO_ASSERTE(!filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(filesystem::create_file(TEST_FILE));
	TOGO_ASSERTE(filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(!filesystem::create_file(TEST_FILE));
	TOGO_ASSERTE(filesystem::create_file(TEST_FILE, true));
	TOGO_ASSERTE(
		filesystem::time_last_modified(TEST_FILE) >
		system::secs_since_epoch() - 2
	);
	TOGO_ASSERTE(filesystem::file_size(TEST_FILE) == 0);

	// File copy
	TOGO_ASSERTE(filesystem::copy_file(TEST_EXEC_FILE, TEST_FILE_COPIED));
	TOGO_ASSERTE(filesystem::is_file(TEST_FILE_COPIED));
	TOGO_ASSERTE(filesystem::file_size(TEST_EXEC_FILE) == filesystem::file_size(TEST_FILE_COPIED));

	TOGO_ASSERTE(!filesystem::copy_file(TEST_FILE, TEST_FILE_COPIED));
	TOGO_ASSERTE(filesystem::copy_file(TEST_FILE, TEST_FILE_COPIED, true));
	TOGO_ASSERTE(filesystem::file_size(TEST_FILE) == filesystem::file_size(TEST_FILE_COPIED));

	// File move
	TOGO_ASSERTE(filesystem::move_file(TEST_FILE, TEST_FILE_MOVED));
	TOGO_ASSERTE(!filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(filesystem::is_file(TEST_FILE_MOVED));
	TOGO_ASSERTE(!filesystem::move_file(TEST_FILE, TEST_FILE_MOVED));

	TOGO_ASSERTE(filesystem::move_file(TEST_FILE_MOVED, TEST_FILE));
	TOGO_ASSERTE(filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(!filesystem::is_file(TEST_FILE_MOVED));

	// File removal
	TOGO_ASSERTE(filesystem::remove_file(TEST_FILE));
	TOGO_ASSERTE(!filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(!filesystem::remove_file(TEST_FILE));
	TOGO_ASSERTE(filesystem::remove_file(TEST_FILE, true));
	TOGO_ASSERTE(filesystem::file_size(TEST_FILE) == 0);

	TOGO_ASSERTE(filesystem::remove_file(TEST_FILE_COPIED));
	TOGO_ASSERTE(!filesystem::is_file(TEST_FILE_COPIED));
	TOGO_ASSERTE(!filesystem::remove_file(TEST_FILE_COPIED));

	// Directory removal
	TOGO_ASSERTE(filesystem::is_empty_directory(TEST_DIR, false));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR));
	TOGO_ASSERTE(filesystem::is_empty_directory(TEST_DIR, true));
	TOGO_ASSERTE(!filesystem::is_empty_directory(TEST_DIR, false));
	TOGO_ASSERTE(!filesystem::is_directory(TEST_DIR));
	TOGO_ASSERTE(!filesystem::remove_directory(TEST_DIR));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR, true));

	// Path creation
	TOGO_ASSERTE(!filesystem::create_directory_whole(""));
	TOGO_ASSERTE(filesystem::create_directory_whole("/"));
#ifdef TOGO_PLATFORM_LINUX
	// (not as root user!)
	TOGO_ASSERTE(!filesystem::create_directory_whole("/__togo-fs-test__"));
#endif
	TOGO_ASSERTE(filesystem::create_directory_whole(TEST_DIR_WHOLE));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE));

	TOGO_ASSERTE(filesystem::create_directory_whole(TEST_DIR_WHOLE));
	TOGO_ASSERTE(filesystem::create_directory_whole(TEST_DIR_WHOLE));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE));

	TOGO_ASSERTE(filesystem::create_directory_whole("./" TEST_DIR_WHOLE));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE));

	TOGO_ASSERTE(filesystem::create_directory_whole("./" TEST_DIR_WHOLE "/a/b/c/"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a/b/c"));

	TOGO_ASSERTE(filesystem::create_directory_whole("./" TEST_DIR_WHOLE "/a/b/c"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a/b/c"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a/b"));

	TOGO_ASSERTE(filesystem::create_directory_whole(TEST_DIR_WHOLE "/a/b/c/"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a/b/c"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a/b"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a"));

	TOGO_ASSERTE(filesystem::create_directory_whole(TEST_DIR_WHOLE "/a/b/c"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a/b/c"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a/b"));
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE "/a"));

	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR_WHOLE));

	return 0;
}
