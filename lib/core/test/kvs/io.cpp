
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/vector/1.hpp>
#include <togo/core/math/vector/2.hpp>
#include <togo/core/math/vector/3.hpp>
#include <togo/core/math/vector/4.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/io/memory_stream.hpp>
#include <togo/core/kvs/kvs.hpp>

#include <togo/support/test.hpp>

#include <cmath>

using namespace togo;

static constexpr char const
	S_EMPTY[] = "",
	S_NULL[] = "x = null",
	S_INTEGER[] = "x = 42",
	S_DECIMAL[] = "x = 3.1415926",
	S_BOOLEAN_FALSE[] = "x = false",
	S_BOOLEAN_TRUE[] = "x = true",
	S_STRING1[] = "x = `````string1```",
	S_STRING2[] = "x = \"```string2```\"",
	S_STRING3[] = "x = \"0string3\"",
	S_STRING4[] = "x = \".string4\"",
	S_STRING5[] = "x = \"\"",
	S_STRING6[] = "x = ``````",
	S_STRING_QNONE[] = "x = non-quoted",
	S_STRING_QDOUBLE[] = "x = \"double-quoted\"",
	S_STRING_QBLOCK[] = "x = ```block-quoted```",
	S_VEC1[] = "x = (0)",
	S_VEC2[] = "x = (0 1)",
	S_VEC3[] = "x = (0 1 2)",
	S_VEC4[] = "x = (0 1 2 3)",

	S_NODE_EMPTY[] = "c = {}",
	S_NODE_X_INTEGER[] = "c = {x = 42}",
	S_NODE_X_ARRAY_EMPTY[] = "c = {x = []}",
	S_NODE_X_ARRAY_COMP[] = "c = {x = [0 1, 2; 3\n4]}",

	S_ARRAY_EMPTY[] = "c = []",
	S_ARRAY_INTEGER[] = "c = [42]",
	S_ARRAY_COMP[] = "c = [0 1, 2; 3\n4]",
	S_ARRAY_COMP_NODE[] = "c = [{}{} {}, {}; {}\n{}]",
	S_ARRAY_COMP_ARRAY[] = "c = [[][] [], []; []\n[]]",

	S_NAME1[] = "\"name1\" = null",
	S_NAME2[] = "```name2``` = null",

	S_COMMENT1[] = "//",
	S_COMMENT2[] = "/**/",
	S_COMMENT3[] = "a = [0/**/1]",
	S_COMMENT4[] = "a = [0//\n1]",
	S_COMMENT5[] = "a = [0/*\n*/1]",

	S_V_INTEGER[] = "v=[0 -1 +1]",
	S_V_DECIMAL[] = "v=[0.0 -1.0 +1.0  0e0 1e-1 1e+1 .0 -.1 +.1 .0e0 -.1e-1 +.1e+1]",
	S_V_STRING[] = "v=[_ A Z a z t0,t1;t2\nt3 \"\" `````` \"\\t\"]",
	S_V_VECTOR[] = "v=[(0)(0.0 -1.0 +1.0) (0e0 1e-1 1e+1),(.0 -.1 +.1);(.0e0)\n(-.1e-1 +.1e+1)]",

	S_E_X_EOF[] = "x",
	S_E_X_ASSIGN_EOF[] = "x = ",

	S_E_INTEGER1[] = "x = -",
	S_E_INTEGER2[] = "x = +",

	S_E_DECIMAL1[] = "x = .",
	S_E_DECIMAL2[] = "x = 0e",
	S_E_DECIMAL3[] = "x = 0e-",
	S_E_DECIMAL4[] = "x = 0e+",
	S_E_DECIMAL5[] = "x = .e",
	S_E_DECIMAL6[] = "x = .0e",
	S_E_DECIMAL7[] = "x = 0.",

	S_E_STRING1[] = "x = \"",
	S_E_STRING2[] = "x = ```",
	S_E_STRING3[] = "x = ``",
	S_E_STRING4[] = "x = `",

	S_E_VECTOR1[] = "x = ()",
	S_E_VECTOR2[] = "x = (",
	S_E_VECTOR3[] = ")",

	S_E_NODE1[] = "x = {",
	S_E_NODE2[] = "}",

	S_E_ARRAY1[] = "x = [",
	S_E_ARRAY2[] = "]",

	S_E_COMMENT1[] = "/",
	S_E_COMMENT2[] = "/*",
	S_E_COMMENT3[] = "x = //",
	S_E_COMMENT4[] = "x = /**/"
;

void test_str(KVS& root, StringRef const& data, bool const expected = true) {
	bool success = true;
	ParserInfo pinfo;
	MemoryReader in_stream{data};
	TOGO_LOGF(
		"reading (%3u): <%.*s>\n",
		data.size, data.size, data.data
	);
	if (kvs::read_text(root, in_stream, pinfo)) {
		MemoryStream out_stream{memory::default_allocator(), data.size + 128};
		TOGO_ASSERTE(kvs::write_text(root, out_stream));
		unsigned size = static_cast<unsigned>(out_stream.size());
		if (size > 0) {
			--size;
		}
		TOGO_LOGF(
			"root rewritten (%3u): <%.*s>\n",
			size, size, array::begin(out_stream.data())
		);
	} else {
		TOGO_LOGF(
			"failed to read (%s): [%2u,%2u]: %s\n",
			expected ? "UNEXPECTED" : "expected",
			pinfo.line,
			pinfo.column,
			pinfo.message
		);
		success = false;
	}
	TOGO_LOG("\n");
	TOGO_ASSERTE(success == expected);
}

signed main() {
	memory_init();

	{
		KVS root;
		test_str(root, S_EMPTY);
		TOGO_ASSERTE(kvs::is_node(root));
		TOGO_ASSERTE(kvs::empty(root));
	}

	// Values
	{
		KVS root;
		StringRef ref;

		test_str(root, S_NULL);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_null(kvs::back(root)));

		test_str(root, S_INTEGER);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_integer(kvs::back(root)));
		TOGO_ASSERTE(kvs::integer(kvs::back(root)) == 42);

		test_str(root, S_DECIMAL);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_decimal(kvs::back(root)));
		TOGO_ASSERTE(std::fabs(kvs::decimal(kvs::back(root)) - 3.1415926) <= 0.01f);

		test_str(root, S_BOOLEAN_FALSE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_boolean(kvs::back(root)));
		TOGO_ASSERTE(kvs::boolean(kvs::back(root)) == false);

		test_str(root, S_BOOLEAN_TRUE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_boolean(kvs::back(root)));
		TOGO_ASSERTE(kvs::boolean(kvs::back(root)) == true);

		test_str(root, S_STRING1);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, "``string1"));

		test_str(root, S_STRING2);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, "```string2```"));

		test_str(root, S_STRING3);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, "0string3"));

		test_str(root, S_STRING4);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, ".string4"));

		test_str(root, S_STRING5);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, ""));

		test_str(root, S_STRING6);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, ""));

		test_str(root, S_STRING_QNONE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, "non-quoted"));

		test_str(root, S_STRING_QDOUBLE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, "double-quoted"));

		test_str(root, S_STRING_QBLOCK);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(string::compare_equal(ref, "block-quoted"));

		test_str(root, S_VEC1);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_vec1(kvs::back(root)));
		TOGO_ASSERTE(kvs::vec1(kvs::back(root)) == Vec1(0));

		test_str(root, S_VEC2);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_vec2(kvs::back(root)));
		TOGO_ASSERTE(kvs::vec2(kvs::back(root)) == Vec2(0, 1));

		test_str(root, S_VEC3);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_vec3(kvs::back(root)));
		TOGO_ASSERTE(kvs::vec3(kvs::back(root)) == Vec3(0, 1, 2));

		test_str(root, S_VEC4);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_vec4(kvs::back(root)));
		TOGO_ASSERTE(kvs::vec4(kvs::back(root)) == Vec4(0, 1, 2, 3));
	}

	// Node
	{
		KVS root;
		test_str(root, S_NODE_EMPTY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_node(kvs::back(root)));
		TOGO_ASSERTE(kvs::empty(kvs::back(root)));

		test_str(root, S_NODE_X_INTEGER);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_node(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 1);
		TOGO_ASSERTE(kvs::is_integer(kvs::back(kvs::back(root))) == 1);
		TOGO_ASSERTE(kvs::integer(kvs::back(kvs::back(root))) == 42);

		test_str(root, S_NODE_X_ARRAY_EMPTY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_node(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(kvs::back(root))) == 1);
		TOGO_ASSERTE(kvs::empty(kvs::back(kvs::back(root))));

		test_str(root, S_NODE_X_ARRAY_COMP);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_node(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(kvs::back(root))) == 1);
		TOGO_ASSERTE(kvs::size(kvs::back(kvs::back(root))) == 5);
		unsigned index = 0;
		for (auto const& child : kvs::back(kvs::back(root))) {
			TOGO_ASSERTE(kvs::is_integer(child));
			TOGO_ASSERTE(kvs::integer(child) == index);
			++index;
		}
	}

	// Array
	{
		KVS root;
		test_str(root, S_ARRAY_EMPTY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::empty(kvs::back(root)));

		test_str(root, S_ARRAY_INTEGER);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 1);
		TOGO_ASSERTE(kvs::is_integer(kvs::back(kvs::back(root))));
		TOGO_ASSERTE(kvs::integer(kvs::back(kvs::back(root))) == 42);

		test_str(root, S_ARRAY_COMP);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 5);
		unsigned index = 0;
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_integer(child));
			TOGO_ASSERTE(kvs::integer(child) == index);
			++index;
		}

		test_str(root, S_ARRAY_COMP_NODE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 6);
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_node(child));
			TOGO_ASSERTE(kvs::empty(child));
		}

		test_str(root, S_ARRAY_COMP_ARRAY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 6);
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_array(child));
			TOGO_ASSERTE(kvs::empty(child));
		}
	}

	// Names
	{
		KVS root;

		test_str(root, S_NAME1);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(string::compare_equal(kvs::name_ref(kvs::back(root)), "name1"));

		test_str(root, S_NAME2);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(string::compare_equal(kvs::name_ref(kvs::back(root)), "name2"));
	}

	// Comments
	{
		KVS root;

		test_str(root, S_COMMENT1);
		TOGO_ASSERTE(kvs::empty(root));

		test_str(root, S_COMMENT2);
		TOGO_ASSERTE(kvs::empty(root));

		test_str(root, S_COMMENT3);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 2);
		unsigned index = 0;
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_integer(child));
			TOGO_ASSERTE(kvs::integer(child) == index);
			++index;
		}

		test_str(root, S_COMMENT4);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 2);
		index = 0;
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_integer(child));
			TOGO_ASSERTE(kvs::integer(child) == index);
			++index;
		}

		test_str(root, S_COMMENT5);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 2);
		index = 0;
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_integer(child));
			TOGO_ASSERTE(kvs::integer(child) == index);
			++index;
		}
	}

	// Variations
	{
		KVS root;

		test_str(root, S_V_INTEGER);
		test_str(root, S_V_DECIMAL);
		test_str(root, S_V_STRING);
		test_str(root, S_V_VECTOR);
	}

	// Errors
	{
		KVS root;

		test_str(root, S_E_X_EOF, false);
		test_str(root, S_E_X_ASSIGN_EOF, false);

		test_str(root, S_E_INTEGER1, false);
		test_str(root, S_E_INTEGER2, false);

		test_str(root, S_E_DECIMAL1, false);
		test_str(root, S_E_DECIMAL2, false);
		test_str(root, S_E_DECIMAL3, false);
		test_str(root, S_E_DECIMAL4, false);
		test_str(root, S_E_DECIMAL5, false);
		test_str(root, S_E_DECIMAL6, false);
		test_str(root, S_E_DECIMAL7, false);

		test_str(root, S_E_STRING1, false);
		test_str(root, S_E_STRING2, false);
		test_str(root, S_E_STRING3, false);
		test_str(root, S_E_STRING4, false);

		test_str(root, S_E_VECTOR1, false);
		test_str(root, S_E_VECTOR2, false);
		test_str(root, S_E_VECTOR3, false);

		test_str(root, S_E_NODE1, false);
		test_str(root, S_E_NODE2, false);

		test_str(root, S_E_ARRAY1, false);
		test_str(root, S_E_ARRAY2, false);

		test_str(root, S_E_COMMENT1, false);
		test_str(root, S_E_COMMENT2, false);
		test_str(root, S_E_COMMENT3, false);
		test_str(root, S_E_COMMENT4, false);
	}

	return 0;
}
