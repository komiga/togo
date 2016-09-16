
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
#include <togo/core/system/system.hpp>
#include <togo/core/parser/types.hpp>
#include <togo/core/parser/parser.hpp>
#include <togo/core/kvs/kvs.hpp>

#include <togo/support/test.hpp>

#include <cmath>
#include <cstdint>

using namespace togo;

/*

TSE("",
R"K(
)K",
FuncSingle{
})

*/

void test_no_op(KVS&, KVS&) {}
#define FuncSingle [](KVS&  , KVS& k) -> void
#define FuncMany   [](KVS& r, KVS&  ) -> void

#define CHECK_NAME(k_, name_) do { \
	TOGO_ASSERTE(string::compare_equal(name_, kvs::name_ref(k_))); \
} while (false)

#define CHECK_TYPE(k_, type_) do { \
	TOGO_ASSERTE(kvs::is_##type_ (k_)); \
} while (false)

#define CHECK_VALUE(k_, name_, type_) do { \
	CHECK_NAME(k_, name_); \
	CHECK_TYPE(k_, type_); \
} while (false)

#define CHECK_COLLECTION(k_, name_, type_, size_) do { \
	CHECK_VALUE(k_, name_, type_); \
	TOGO_ASSERTE(kvs::size(k_) == size_); \
} while (false)

#define TSN(i)			{true , __LINE__, i, {}, test_no_op},
#define TSS(i, f)		{true , __LINE__, i, i , f},
#define TSE(i, e, f)	{true , __LINE__, i, e , f},
#define TF(i)			{false, __LINE__, i, {}, test_no_op},
#define TSS_F(i)		{true , __LINE__, i, i , test_no_op},
#define TSE_F(i, e)		{true , __LINE__, i, e , test_no_op},

struct Test {
	using Func = void (KVS& r, KVS& k);

	bool expected_success;
	unsigned line;
	StringRef input;
	StringRef expected_output;
	Func* func;
} const s_tests[]{

TSN("")
TSN("// comment")
TSN("/* block comment */")

// name
TSS("x = null",
FuncSingle{
	CHECK_VALUE(k, "x", null);
})
TSE("\"x\" = null",
	"x = null",
FuncSingle{
	CHECK_VALUE(k, "x", null);
})
TSE("```x``` = null",
	"x = null",
FuncSingle{
	CHECK_VALUE(k, "x", null);
})

TSS("\"\tx\" = null",
FuncSingle{
	CHECK_VALUE(k, "\tx", null);
})
TSS("```x\n``` = null",
FuncSingle{
	CHECK_VALUE(k, "x", null);
})

// values
TSS("x = 42",
FuncSingle{
	CHECK_VALUE(k, "x", integer);
	TOGO_ASSERTE(kvs::integer(k) == 42);
})
TSE("x = 3.1415926",
	"x = 3.1415926",
FuncSingle{
	CHECK_VALUE(k, "x", decimal);
	TOGO_ASSERTE(std::fabs(kvs::decimal(k) - 3.1415926f) <= 0.01f);
})
TSE("x = 42401.3536",
	"x = 42401.3536000",
FuncSingle{
	CHECK_VALUE(k, "x", decimal);
	TOGO_ASSERTE(std::fabs(kvs::decimal(k) - 42401.3536f) <= 0.01f);
})
TSS("x = false",
FuncSingle{
	CHECK_VALUE(k, "x", boolean);
	TOGO_ASSERTE(kvs::boolean(k) == false);
})
TSS("x = true",
FuncSingle{
	CHECK_VALUE(k, "x", boolean);
	TOGO_ASSERTE(kvs::boolean(k) == true);
})

TSS("x = \"\"",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("", kvs::string_ref(k)));
})
TSE("x = ``````",
	"x = \"\"",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("", kvs::string_ref(k)));
})

TSS("x = non-quoted",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("non-quoted", kvs::string_ref(k)));
})
TSE("x = \"double-quoted\"",
	"x = double-quoted",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("double-quoted", kvs::string_ref(k)));
})
TSE("x = ```block-quoted```",
	"x = block-quoted",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("block-quoted", kvs::string_ref(k)));
})

TSS("x = \"0string3\"",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("0string3", kvs::string_ref(k)));
})
TSS("x = \".string4\"",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal(".string4", kvs::string_ref(k)));
})
TSE("x = `````string```",
	"x = \"``string\"",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("``string", kvs::string_ref(k)));
})
TSS("x = \"```string```\"",
FuncSingle{
	CHECK_VALUE(k, "x", string);
	TOGO_ASSERTE(string::compare_equal("```string```", kvs::string_ref(k)));
})

TSS("x = (0)",
	// "x = (0.0)",
FuncSingle{
	CHECK_VALUE(k, "x", vec1);
	TOGO_ASSERTE(kvs::vec1(k) == Vec1(0));
})
TSS("x = (0 1)",
	// "x = (0.0 1.0)",
FuncSingle{
	CHECK_VALUE(k, "x", vec2);
	TOGO_ASSERTE(kvs::vec2(k) == Vec2(0, 1));
})
TSS("x = (0 1 2)",
	// "x = (0.0 1.0 2.0)",
FuncSingle{
	CHECK_VALUE(k, "x", vec3);
	TOGO_ASSERTE(kvs::vec3(k) == Vec3(0, 1, 2));
})
TSS("x = (0 1 2 3)",
	// "x = (0.0 1.0 2.0 3.0)",
FuncSingle{
	CHECK_VALUE(k, "x", vec4);
	TOGO_ASSERTE(kvs::vec4(k) == Vec4(0, 1, 2, 3));
})

// collections
TSS("c = {}",
FuncSingle{
	CHECK_COLLECTION(k, "c", node, 0);
})

TSE("c = {x = 42}",
R"K(c = {
	x = 42
})K",
FuncSingle{
	CHECK_COLLECTION(k, "c", node, 1);
	{
		KVS& x = k[0];
		CHECK_VALUE(x, "x", integer);
		TOGO_ASSERTE(kvs::integer(x) == 42);
	}
})

TSE("c = {x = []}",
R"K(c = {
	x = []
})K",
FuncSingle{
	CHECK_COLLECTION(k, "c", node, 1);
	{
		KVS& x = k[0];
		CHECK_COLLECTION(x, "x", array, 0);
	}
})

TSE("x = [42]",
R"K(x = [
	42
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 1);
	{
		KVS& x = k[0];
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == 42);
	}
})

TSE("c = [0 1, 2; 3\n4]",
R"K(c = [
	0
	1
	2
	3
	4
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 4);
	unsigned i = 0;
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == i);
		++i;
	}
})

TSE("c = [{}{} {}, {}; {}\n{}]",
R"K(c = [
	{}
	{}
	{}
	{}
	{}
	{}
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 6);
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, node);
		TOGO_ASSERTE(kvs::size(x) == 0);
	}
})

TSE("c = [[][] [], []; []\n[]]",
R"K(c = [
	[]
	[]
	[]
	[]
	[]
	[]
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 6);
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, array);
		TOGO_ASSERTE(kvs::size(x) == 0);
	}
})

TSE("c = [0// comment\n1]",
R"K(c = [
	0
	1
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 2);
	unsigned i = 0;
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == i);
		++i;
	}
})

TSE("c = [0/* comment */1]",
R"K(c = [
	0
	1
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 2);
	unsigned i = 0;
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == i);
		++i;
	}
})

TSE("c = [0/*\n\tcomment\n*/1]",
R"K(c = [
	0
	1
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 2);
	unsigned i = 0;
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == i);
		++i;
	}
})

// value permutations

TSE("c = [0 -1 +1]",
R"K(c = [
	0
	-1
	1
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 2);
	{
		KVS& x = k[0];
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == 0);
	}
	{
		KVS& x = k[1];
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == -1);
	}
	{
		KVS& x = k[2];
		CHECK_TYPE(x, integer);
		TOGO_ASSERTE(kvs::integer(x) == 1);
	}
})

TSE("c = [0.0 -1.0 +1.0  0e0 1e-1 1e+1 .0 -.1 +.1 .0e0 -.1e-1 +.1e+1]",
R"K(c = [
	0.0000000
	-1.0000000
	1.0000000
	0.0000000
	0.1000000
	10.0000000
	0.0000000
	-0.1000000
	0.1000000
	0.0000000
	-0.0100000
	1.0000000
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 2);
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, decimal);
	}
})

TSE("c = [_ A Z a z t0,t1;t2\nt3 \"\" `````` \"\\t\"]",
R"K(c = [
	_
	A
	Z
	a
	z
	t0
	t1
	t2
	t3
	""
	""
	"	"
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 2);
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, string);
	}
})

TSE("c = [(0)(0.0 -1.0 +1.0) (0e0 1e-1 1e+1),(.0 -.1 +.1);(.0e0)\n(-.1e-1 +.1e+1)]",
R"K(c = [
	(0)
	(0 -1 1)
	(0 0.1 10)
	(0 -0.1 0.1)
	(0)
	(-0.01 1)
])K",
FuncSingle{
	CHECK_COLLECTION(k, "c", array, 2);
	for (KVS& x : kvs::back(k)) {
		CHECK_TYPE(x, decimal);
	}
})

TF("x")
TF("x = ")
TF("x = -")
TF("x = +")
TF("x = .")
TF("x = 0e")
TF("x = 0e-")
TF("x = 0e+")
TF("x = .e")
TF("x = .0e")
TF("x = 0.")
TF("x = \"")
TF("x = ```")
TF("x = ``")
TF("x = `")

TF("x = ()")
TF("x = (")
TF(")")
TF("x = {")
TF("}")

TF("x = [")
TF("]")

TF("/")
TF("/*")
TF("x = //")
TF("x = /**/")

};

void do_test_old(Test const& test) {
	TOGO_LOGF(
		"!! old  %-3u @ %3u: <%.*s>\n",
		test.input.size,
		test.line,
		test.input.size, test.input.data
	);
	KVS root;
	KVSParserInfo pinfo{};
	MemoryReader in_stream{test.input};
	if (kvs::read_text(root, in_stream, pinfo)) {
		MemoryStream out_stream{memory::default_allocator(), test.expected_output.size + 1};
		TOGO_ASSERTE(kvs::write_text(root, out_stream));
		StringRef output{
			reinterpret_cast<char*>(array::begin(out_stream.data())),
			static_cast<unsigned>(out_stream.size())
		};
		if (output.size > 0) {
			--output.size;
		}
		TOGO_LOGF(
			"rewritten (%3u): <%.*s>\n",
			output.size, output.size, output.data
		);
		TOGO_ASSERT(test.expected_success, "read succeeded when it should have failed");
		TOGO_ASSERT(
			string::compare_equal(test.expected_output, output),
			"output does not match"
		);
	} else {
		TOGO_LOGF(
			"failed to read%s: [%2u,%2u]: %s\n",
			test.expected_success ? " (UNEXPECTED)" : "",
			pinfo.line,
			pinfo.column,
			pinfo.message
		);
		TOGO_ASSERT(!test.expected_success, "read failed when it should have succeeded");
	}
	if (test.func) {
		KVS& first = kvs::any(root) ? root[0] : root;
		test.func(root, first);
	}
	TOGO_LOG("\n");
}

void do_test_new(Test const& test) {
	TOGO_LOGF(
		"!! new  %-3u @ %3u\n",
		test.input.size,
		test.line
	);
	KVS root;
	ParseError error{};
	if (kvs::read_text_new(root, array_cref(test.input), &error)) {
		MemoryStream out_stream{memory::default_allocator(), test.expected_output.size + 1};
		TOGO_ASSERTE(kvs::write_text(root, out_stream));
		StringRef output{
			reinterpret_cast<char*>(array::begin(out_stream.data())),
			static_cast<unsigned>(out_stream.size())
		};
		if (output.size > 0) {
			--output.size;
		}
		/*TOGO_LOGF(
			"rewritten (%3u): <%.*s>\n",
			output.size, output.size, output.data
		);*/
		TOGO_ASSERT(test.expected_success, "read succeeded when it should have failed");
		TOGO_ASSERT(
			string::compare_equal(test.expected_output, output),
			"output does not match"
		);
	} else {
		TOGO_LOGF(
			"failed to read%s: [%2u,%2u]: %.*s\n",
			test.expected_success ? " (UNEXPECTED)" : "",
			error.line,
			error.column,
			string::size(error.message), begin(error.message)
		);
		TOGO_ASSERT(!test.expected_success, "read failed when it should have succeeded");
	}
	if (test.func) {
		KVS& first = kvs::any(root) ? root[0] : root;
		test.func(root, first);
	}
	TOGO_LOG("\n");
}

using MeasureFunc = void();

f64 measure(StringRef group, u64 num, MeasureFunc f) {
	f64 start = system::time_monotonic();
	for (unsigned i = num; i; i--) {
		f();
	}
	f64 end = system::time_monotonic();
	f64 duration = end - start;
	f64 average = duration / static_cast<f64>(num);
	TOGO_LOGF(
		"group %.*s: num = %lu time = %12.06lf  average = %-6.08f\n",
		group.size, group.data,
		num, duration, average
	);
	return duration;
}

signed main(signed argc, char* argv[]) {
	memory_init();

	#if defined(TOGO_DEBUG)
		parser::s_debug_trace = false;
	#endif

	if (argc > 1) {
		f64 num_float = std::strtod(argv[1], nullptr);
		u64 num;
		if (num_float == 0.0f || num_float == HUGE_VALF) {
			num = 1e4;
		} else {
			num = static_cast<u64>(num_float);
		}
		f64 duration_old = measure("old", num, [](){
			KVS root;
			KVSParserInfo pinfo;
			MemoryReader in_stream{""};
			for (auto& test : s_tests) {
				new (&in_stream) MemoryReader(test.input);
				kvs::read_text(root, in_stream, pinfo);
			}
		});
		f64 duration_new = measure("new", num, [](){
			KVS root;
			ParseError error{};
			for (auto& test : s_tests) {
				kvs::read_text_new(root, array_cref(test.input), &error);
			}
		});

		f64 ratio = duration_new / duration_old;
		f64 ratio_log2 = std::log2(ratio);
		TOGO_LOGF(
			"ratio (new : old) = %-6.03lf  log2(ratio) = %-6.03f\n",
			ratio, ratio_log2
		);
	} else {
		for (auto& test : s_tests) {
			do_test_old(test);
			do_test_new(test);
		}
	}

	return 0;
}
