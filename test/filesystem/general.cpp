
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/fixed_array.hpp>
#include <togo/string.hpp>
#include <togo/system.hpp>
#include <togo/filesystem.hpp>

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
		TOGO_ASSERTE(filesystem::is_file("general.elf"));
	#elif defined(TOGO_PLATFORM_WINDOWS)
		TOGO_ASSERTE(filesystem::is_file("general.exe"));
	#endif

	TOGO_ASSERTE(!filesystem::is_directory("/non/existent/directory"));
	TOGO_ASSERTE( filesystem::is_directory("."));
	TOGO_ASSERTE( filesystem::is_directory(".."));

	#define TEST_DIR "test_dir"
	#define TEST_FILE TEST_DIR "/test_file"

	// Directory creation
	TOGO_ASSERTE(!filesystem::is_directory(TEST_DIR));
	TOGO_ASSERTE(filesystem::create_directory(TEST_DIR));
	TOGO_ASSERTE(filesystem::is_directory(TEST_DIR));
	TOGO_ASSERTE(!filesystem::create_directory(TEST_DIR));
	TOGO_ASSERTE(
		filesystem::time_last_modified(TEST_DIR) >
		system::secs_since_epoch() - 2
	);

	// File creation
	TOGO_ASSERTE(!filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(filesystem::create_file(TEST_FILE));
	TOGO_ASSERTE(filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(!filesystem::create_file(TEST_FILE));
	TOGO_ASSERTE(
		filesystem::time_last_modified(TEST_FILE) >
		system::secs_since_epoch() - 2
	);

	// File removal
	TOGO_ASSERTE(filesystem::remove_file(TEST_FILE));
	TOGO_ASSERTE(!filesystem::is_file(TEST_FILE));
	TOGO_ASSERTE(!filesystem::remove_file(TEST_FILE));

	// Directory removal
	TOGO_ASSERTE(filesystem::remove_directory(TEST_DIR));
	TOGO_ASSERTE(!filesystem::is_directory(TEST_DIR));
	TOGO_ASSERTE(!filesystem::remove_directory(TEST_DIR));

	return 0;
}
