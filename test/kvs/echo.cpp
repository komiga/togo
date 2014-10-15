
#include <togo/assert.hpp>
#include <togo/string.hpp>
#include <togo/log.hpp>
#include <togo/array.hpp>
#include <togo/kvs.hpp>
#include <togo/io.hpp>
#include <togo/memory_io.hpp>

#include "../common/helpers.hpp"

#include <cmath>
#include <cstring>

using namespace togo;

void echo(KVS& root, StringRef const& path) {
	TOGO_LOGF("#### reading file: '%.*s'\n", path.size, path.data);
	if (kvs::read_file(root, path)) {
		MemoryStream out_stream{memory::default_allocator(), 2048};
		TOGO_ASSERTE(kvs::write(root, out_stream));
		unsigned const size = static_cast<unsigned>(
			array::size(out_stream.buffer())
		);
		TOGO_LOGF(
			"#### rewritten (%u): <%.*s>\n",
			size, size, array::begin(out_stream.buffer())
		);
	} else {
		TOGO_LOG_ERROR("#### failed to read file\n");
	}
	TOGO_LOG("\n");
}

signed main(signed argc, char* argv[]) {
	memory_init();
	KVS root;
	for (signed i = 1; i < argc; ++i) {
		echo(root, {argv[i], cstr_tag{}});
	}
	return 0;
}
