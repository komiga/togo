
#include <togo/assert.hpp>
#include <togo/string.hpp>

using namespace togo;

signed
main() {
	TOGO_ASSERTE(string::compare_equal("", ""));
	TOGO_ASSERTE(string::compare_equal("a", "a"));
	TOGO_ASSERTE(string::compare_equal("Xyzzy", "Xyzzy"));

	TOGO_ASSERTE(!string::compare_equal("", "a"));
	TOGO_ASSERTE(!string::compare_equal("a", ""));
	TOGO_ASSERTE(!string::compare_equal("a", "b"));
	TOGO_ASSERTE(!string::compare_equal("aa", "a"));

	return 0;
}
