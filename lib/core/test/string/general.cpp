
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/string/string.hpp>

#include <cstring>

using namespace togo;

#define PATH_TEST(f, path, expected) \
	path_test(#f, string:: f, path, expected)

#define PATH_TEST(f, path, expected) \
	path_test(#f, string:: f, path, expected)

#define PATH_TEST_PARTS(path, dir, file) do { \
	PATH_TEST(path_dir, path, dir); \
	PATH_TEST(path_file, path, file); \
	} while(false)

static StringRef nothing_or(StringRef str) {
	return str.any() ? str : "<>";
}

static void path_test(
	StringRef name,
	StringRef (f)(StringRef const&),
	StringRef path,
	StringRef expected
) {
	StringRef result = f(path);
	StringRef print_result = nothing_or(result);
	TOGO_LOGF(
		"%-10s: %-8s => %.*s\n",
		name.data,
		nothing_or(path).data,
		print_result.size, print_result.data
	);
	TOGO_ASSERTF(
		string::compare_equal(expected, result),
		"  wanted %s",
		nothing_or(expected).data
	);
}

signed main() {
	{
		FixedArray<char, 3> fa;
		fixed_array::resize(fa, 3);
		std::memcpy(fixed_array::begin(fa), "str", 3);
		StringRef const lit_r_1{"str"};
		StringRef const cstr_r_1{"str", cstr_tag{}};
		StringRef const fa_r_1{fa};

		TOGO_ASSERTE(string::compare_equal(lit_r_1, lit_r_1));
		TOGO_ASSERTE(string::compare_equal(lit_r_1, cstr_r_1));
		TOGO_ASSERTE(string::compare_equal(lit_r_1, fa_r_1));
	}

	{
		FixedArray<char, 1> const fa_2;
		StringRef const null_r_2{};
		StringRef const lit_r_2{""};
		StringRef const cstr_r_2{"", cstr_tag{}};
		StringRef const fa_r_2{fa_2};

		TOGO_ASSERTE(string::compare_equal(null_r_2, null_r_2));
		TOGO_ASSERTE(string::compare_equal(null_r_2, lit_r_2));
		TOGO_ASSERTE(string::compare_equal(null_r_2, cstr_r_2));
		TOGO_ASSERTE(string::compare_equal(null_r_2, fa_r_2));

		TOGO_ASSERTE(string::compare_equal("", ""));
		TOGO_ASSERTE(string::compare_equal("a", "a"));
		TOGO_ASSERTE(string::compare_equal("Xyzzy", "Xyzzy"));

		TOGO_ASSERTE(!string::compare_equal("", "a"));
		TOGO_ASSERTE(!string::compare_equal("a", ""));
		TOGO_ASSERTE(!string::compare_equal("a", "b"));
		TOGO_ASSERTE(!string::compare_equal("aa", "a"));
	}

	{
		#define TEST_COPY(dest__, value__) \
			string::copy(dest__, value__); \
			TOGO_ASSERTE(fixed_array::size(dest__) == value__.size + 1); \
			TOGO_ASSERTE(string::compare_equal(dest__, value__))

		StringRef const empty1{};
		StringRef const empty2{""};
		StringRef const value1{"abc"};

		FixedArray<char, 4> dest1;

		TEST_COPY(dest1, empty1);
		TEST_COPY(dest1, empty2);
		TEST_COPY(dest1, value1);

		#undef TEST_COPY
	}

	{
		#define TEST_APPEND(init__, str__, res__) { \
			char ca[array_extent(res__)]{init__}; \
			FixedArray<char, array_extent(res__)> fa; \
			string::copy(fa, init__); \
			string::append(ca, string::size_literal(init__), str__); \
			string::append(fa, str__); \
			unsigned const res_size = string::size_literal(res__); \
			unsigned const ca_size = string::size(ca); \
			unsigned const fa_size = string::size(fa); \
			TOGO_ASSERTE(ca_size == res_size); \
			TOGO_ASSERTE(fa_size == res_size); \
			TOGO_ASSERTE(string::compare_equal(res__, {ca, ca_size})); \
			TOGO_ASSERTE(string::compare_equal(res__, fa)); \
			TOGO_ASSERTE(string::compare_equal(fa, {ca, ca_size})); \
		}

		TEST_APPEND("", "", "");
		TEST_APPEND("", "a", "a");
		TEST_APPEND("a", "b", "ab");
		TEST_APPEND("a", "", "a");

		#undef TEST_APPEND
	}

	{
		#define TEST_TRIM(str__, res__) { \
			char ca[array_extent(str__)]{str__}; \
			FixedArray<char, array_extent(str__)> fa{}; \
			string::copy(fa, str__); \
			unsigned const res_size = string::size_literal(res__); \
			unsigned const ca_size = string::trim_trailing_slashes(ca, string::size(ca)); \
			unsigned const fa_size = string::trim_trailing_slashes(fa); \
			TOGO_ASSERTE(ca_size == res_size); \
			TOGO_ASSERTE(fa_size == res_size); \
			TOGO_ASSERTE(string::compare_equal(res__, {ca, ca_size})); \
			TOGO_ASSERTE(string::compare_equal(res__, fa)); \
			TOGO_ASSERTE(string::compare_equal(fa, {ca, ca_size})); \
		}

		TEST_TRIM("", "");
		TEST_TRIM("a", "a");
		TEST_TRIM("/", "");
		TEST_TRIM("//", "");
		TEST_TRIM("/a/", "/a");
		TEST_TRIM("/a//", "/a");

		#undef TEST_TRIM
	}

	// Paths
	PATH_TEST_PARTS(""       , ""    , ""   );
	PATH_TEST_PARTS("."      , "."   , ""   );
	PATH_TEST_PARTS(".."     , ".."  , ""   );
	PATH_TEST_PARTS("/."     , "/."  , ""   );
	PATH_TEST_PARTS("/.."    , "/.." , ""   );
	PATH_TEST_PARTS("c"      , ""    , "c"  );
	PATH_TEST_PARTS("a/"     , "a"   , ""   );
	PATH_TEST_PARTS("/"      , "/"   , ""   );
	PATH_TEST_PARTS("/a"     , "/"   , "a"  );
	PATH_TEST_PARTS("a/b/c"  , "a/b" , "c"  );
	PATH_TEST_PARTS("a/b/c.d", "a/b" , "c.d");

	return 0;
}
