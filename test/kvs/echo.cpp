
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/collection/array.hpp>
#include <togo/string/string.hpp>
#include <togo/io/io.hpp>
#include <togo/io/memory_stream.hpp>
#include <togo/kvs/kvs.hpp>

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
