
#include <togo/memory.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/memory_io.hpp>
#include <togo/kvs.hpp>
#include <togo/args.hpp>
#include <togo/array.hpp>

#include "../common/helpers.hpp"

using namespace togo;

signed main(signed argc, char* argv[]) {
	memory_init();

	KVS options;
	KVS command;
	if (!parse_args(options, command, argc, argv)) {
		TOGO_LOG("#### no command\n");
	}

	{
		MemoryStream out_stream{memory::default_allocator(), 1024};
		TOGO_ASSERTE(kvs::write(options, out_stream));
		TOGO_LOGF(
			"#### options (%u): `%.*s`: <%.*s>\n",
			static_cast<unsigned>(array::size(out_stream.buffer())),
			kvs::name_size(options), kvs::name(options),
			static_cast<unsigned>(array::size(out_stream.buffer())),
			array::begin(out_stream.buffer())
		);
		out_stream.clear();

		TOGO_ASSERTE(kvs::write(command, out_stream));
		TOGO_LOGF(
			"#### command (%u): `%.*s` <%.*s>\n",
			static_cast<unsigned>(array::size(out_stream.buffer())),
			kvs::name_size(command), kvs::name(command),
			static_cast<unsigned>(array::size(out_stream.buffer())),
			array::begin(out_stream.buffer())
		);
	}

	return 0;
}
