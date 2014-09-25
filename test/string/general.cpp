
#include <togo/assert.hpp>
#include <togo/fixed_array.hpp>
#include <togo/string.hpp>

#include <cstring>

using namespace togo;

signed
main() {
	FixedArray<char, 3> fa;
	fixed_array::resize(fa, 3);
	std::memcpy(fixed_array::begin(fa), "str", 3);
	StringRef lit_r_1{"str"};
	StringRef cstr_r_1{"str", cstr_tag{}};
	StringRef fa_r_1{fa};

	TOGO_ASSERTE(string::compare_equal(lit_r_1, lit_r_1));
	TOGO_ASSERTE(string::compare_equal(lit_r_1, cstr_r_1));
	TOGO_ASSERTE(string::compare_equal(lit_r_1, fa_r_1));

	FixedArray<char, 1> fa_2;
	StringRef null_r_2{null_tag{}};
	StringRef lit_r_2{""};
	StringRef cstr_r_2{"", cstr_tag{}};
	StringRef fa_r_2{fa_2};

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

	return 0;
}
