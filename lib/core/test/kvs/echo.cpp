
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/memory_stream.hpp>
#include <togo/core/kvs/kvs.hpp>

#include <togo/support/test.hpp>

#include <cmath>
#include <cstring>

using namespace togo;

void echo(KVS& root, StringRef const& path, bool const binary) {
	TOGO_LOGF("#### reading file: '%.*s'\n", path.size, path.data);
	if (
		binary
		? kvs::read_binary_file(root, path)
		: kvs::read_text_file(root, path)
	) {
		MemoryStream out_stream{memory::default_allocator(), 4096};
		TOGO_ASSERTE(kvs::write_text(root, out_stream));
		unsigned const size = static_cast<unsigned>(out_stream.size());
		TOGO_LOGF(
			"#### rewritten (%u): <%.*s>\n",
			size, size, array::begin(out_stream.data())
		);
	} else {
		TOGO_LOG_ERROR("#### failed to read file\n");
	}
	TOGO_LOG("\n");
}

signed main(signed argc, char* argv[]) {
	memory_init();
	KVS root;
	StringRef ref;
	bool binary = false;
	for (signed i = 1; i < argc; ++i) {
		ref = {argv[i], cstr_tag{}};
		if (string::compare_equal(ref, "-b")) {
			binary = true;
		} else if (string::compare_equal(ref, "-t")) {
			binary = false;
		} else {
			echo(root, ref, binary);
		}
	}
	return 0;
}
