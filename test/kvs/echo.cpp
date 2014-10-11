
#include <togo/assert.hpp>
#include <togo/string.hpp>
#include <togo/log.hpp>
#include <togo/array.hpp>
#include <togo/kvs.hpp>
#include <togo/io.hpp>
#include <togo/memory_io.hpp>
#include <togo/file_io.hpp>

#include "../common/helpers.hpp"

#include <cmath>
#include <cstring>

using namespace togo;

void echo(KVS& root, StringRef const& path) {
	ParserInfo pinfo;
	FileReader stream;
	if (!stream.open(path)) {
		TOGO_LOG_ERRORF("#### failed to open file: '%.*s'\n", path.size, path.data);
		return;
	}
	TOGO_LOGF("#### reading file: '%.*s'\n", path.size, path.data);
	if (kvs::read(root, stream, pinfo)) {
		MemoryStream out_stream{
			memory::default_allocator(),
			static_cast<unsigned>(io::position(stream) + 128)
		};
		TOGO_ASSERTE(kvs::write(root, out_stream));
		TOGO_LOGF(
			"#### rewritten (%u): <%.*s>\n",
			static_cast<unsigned>(array::size(out_stream.buffer())),
			static_cast<unsigned>(array::size(out_stream.buffer())),
			array::begin(out_stream.buffer())
		);
	} else {
		TOGO_LOG_ERRORF(
			"#### failed to read: [%2u,%2u]: %s\n",
			pinfo.line,
			pinfo.column,
			pinfo.message
		);
	}
	TOGO_LOG("\n");
	stream.close();
}

signed main(signed argc, char* argv[]) {
	memory_init();
	KVS root;
	for (signed i = 1; i < argc; ++i) {
		echo(root, {argv[i], cstr_tag{}});
	}
	return 0;
}
