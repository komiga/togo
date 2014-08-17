
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/array.hpp>
#include <togo/kvs.hpp>
#include <togo/memory_io.hpp>

#include "../common/helpers.hpp"

#include <cmath>
#include <cstring>

using namespace togo;

static constexpr char const
	S_EMPTY[] = "",
	S_X_NULL[] = "x = null",
	S_X_INTEGER[] = "x = 42",
	S_X_DECIMAL[] = "x = 3.1415926",
	S_X_BOOLEAN_FALSE[] = "x = false",
	S_X_BOOLEAN_TRUE[] = "x = true",
	S_X_STRING[] = "x = unquoted",
	S_X_STRING_QDOUBLE[] = "x = \"double-quoted\"",
	S_X_STRING_QBLOCK[] = "x = ```block-quoted```",
	/*S_X_VEC1[] = "x = (0)",
	S_X_VEC2[] = "x = (0 1)",
	S_X_VEC3[] = "x = (0 1 2)",
	S_X_VEC4[] = "x = (0 1 2 3)",*/

	S_C_NODE_EMPTY[] = "c = {}",
	S_C_NODE_X_INTEGER[] = "c = {x = 42}",
	S_C_NODE_X_ARRAY_EMPTY[] = "c = {x = []}",
	S_C_NODE_X_ARRAY_COMP[] = "c = {x = [0 1, 2; 3\n4]}",

	S_C_ARRAY_EMPTY[] = "c = []",
	S_C_ARRAY_INTEGER[] = "c = [42]",
	S_C_ARRAY_COMP[] = "c = [0 1, 2; 3\n4]",
	S_C_ARRAY_COMP_NODE[] = "c = [{}{} {}, {}; {}\n{}]",
	S_C_ARRAY_COMP_ARRAY[] = "c = [[][] [], []; []\n[]]",

	S_V_INTEGER[] = "v=[0 -1 +1]",
	S_V_DECIMAL[] = "v=[0.0 -1.0 +1.0  0e0 1e-1 1e+1 .0 -.1 +.1 .0e0 -.1e-1 +.1e+1]",
	S_V_STRING[] = "v=[_ A Z a z t0,t1;t2\nt3 \"\" `````` \"\\t\"]",

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

	S_E_NODE1[] = "x = {",
	S_E_NODE2[] = "}",

	S_E_ARRAY1[] = "x = [",
	S_E_ARRAY2[] = "]"
;

void test_str(KVS& root, StringRef const& data, bool const expected = true) {
	bool success = true;
	ParserInfo pinfo;
	MemoryReader in_stream{data};
	TOGO_LOGF(
		"reading (%u): <%.*s>\n",
		data.size,
		data.size,
		data.data
	);
	if (kvs::read(root, in_stream, pinfo)) {
		MemoryStream out_stream{memory::default_allocator(), data.size + 128};
		TOGO_ASSERTE(kvs::write(root, out_stream));
		TOGO_LOGF(
			"root rewritten (%u): <%.*s>\n",
			static_cast<unsigned>(array::size(out_stream.buffer())),
			static_cast<unsigned>(array::size(out_stream.buffer())),
			array::begin(out_stream.buffer())
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

signed
main() {
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
		test_str(root, S_X_NULL);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_null(kvs::back(root)));

		test_str(root, S_X_INTEGER);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_integer(kvs::back(root)));
		TOGO_ASSERTE(kvs::integer(kvs::back(root)) == 42);

		test_str(root, S_X_DECIMAL);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_decimal(kvs::back(root)));
		TOGO_ASSERTE(std::fabs(kvs::decimal(kvs::back(root)) - 3.1415926) <= 0.01f);

		test_str(root, S_X_BOOLEAN_FALSE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_boolean(kvs::back(root)));
		TOGO_ASSERTE(kvs::boolean(kvs::back(root)) == false);

		test_str(root, S_X_BOOLEAN_TRUE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_boolean(kvs::back(root)));
		TOGO_ASSERTE(kvs::boolean(kvs::back(root)) == true);

		test_str(root, S_X_STRING);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		StringRef ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(std::strncmp("unquoted", ref.data, ref.size) == 0);

		test_str(root, S_X_STRING_QDOUBLE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(std::strncmp("double-quoted", ref.data, ref.size) == 0);

		test_str(root, S_X_STRING_QBLOCK);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_string(kvs::back(root)));
		ref = kvs::string_ref(kvs::back(root));
		TOGO_ASSERTE(std::strncmp("block-quoted", ref.data, ref.size) == 0);
	}

	// Node
	{
		KVS root;
		test_str(root, S_C_NODE_EMPTY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_node(kvs::back(root)));
		TOGO_ASSERTE(kvs::empty(kvs::back(root)));

		test_str(root, S_C_NODE_X_INTEGER);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_node(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 1);
		TOGO_ASSERTE(kvs::is_integer(kvs::back(kvs::back(root))) == 1);
		TOGO_ASSERTE(kvs::integer(kvs::back(kvs::back(root))) == 42);

		test_str(root, S_C_NODE_X_ARRAY_EMPTY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_node(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(kvs::back(root))) == 1);
		TOGO_ASSERTE(kvs::empty(kvs::back(kvs::back(root))));

		test_str(root, S_C_NODE_X_ARRAY_COMP);
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
		test_str(root, S_C_ARRAY_EMPTY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::empty(kvs::back(root)));

		test_str(root, S_C_ARRAY_INTEGER);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 1);
		TOGO_ASSERTE(kvs::is_integer(kvs::back(kvs::back(root))));
		TOGO_ASSERTE(kvs::integer(kvs::back(kvs::back(root))) == 42);

		test_str(root, S_C_ARRAY_COMP);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 5);
		unsigned index = 0;
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_integer(child));
			TOGO_ASSERTE(kvs::integer(child) == index);
			++index;
		}

		test_str(root, S_C_ARRAY_COMP_NODE);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 6);
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_node(child));
			TOGO_ASSERTE(kvs::empty(child));
		}

		test_str(root, S_C_ARRAY_COMP_ARRAY);
		TOGO_ASSERTE(kvs::size(root) == 1);
		TOGO_ASSERTE(kvs::is_array(kvs::back(root)));
		TOGO_ASSERTE(kvs::size(kvs::back(root)) == 6);
		for (auto const& child : kvs::back(root)) {
			TOGO_ASSERTE(kvs::is_array(child));
			TOGO_ASSERTE(kvs::empty(child));
		}
	}

	// Variations
	{
		KVS root;

		test_str(root, S_V_INTEGER);
		test_str(root, S_V_DECIMAL);
		test_str(root, S_V_STRING);
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

		test_str(root, S_E_NODE1, false);
		test_str(root, S_E_NODE2, false);

		test_str(root, S_E_ARRAY1, false);
		test_str(root, S_E_ARRAY2, false);
	}

	return 0;
}
