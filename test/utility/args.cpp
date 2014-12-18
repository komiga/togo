
#include <togo/error/assert.hpp>
#include <togo/utility/args.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/array.hpp>
#include <togo/io/memory_stream.hpp>
#include <togo/kvs/kvs.hpp>

#include "../common/helpers.hpp"

using namespace togo;

signed main(signed argc, char* argv[]) {
	memory_init();

	KVS k_options;
	KVS k_command_options;
	KVS k_command;
	if (!parse_args(k_options, k_command_options, k_command, argc, argv)) {
		TOGO_LOG("#### no command\n");
	}

	{
		MemoryStream out_stream{memory::default_allocator(), 1024};
		TOGO_ASSERTE(kvs::write(k_options, out_stream));
		TOGO_LOGF(
			"#### options (%u): `%.*s`: <%.*s>\n",
			static_cast<unsigned>(array::size(out_stream.buffer())),
			kvs::name_size(k_options), kvs::name(k_options),
			static_cast<unsigned>(array::size(out_stream.buffer())),
			array::begin(out_stream.buffer())
		);
		out_stream.clear();

		TOGO_ASSERTE(kvs::write(k_command_options, out_stream));
		TOGO_LOGF(
			"#### command options (%u): `%.*s` <%.*s>\n",
			static_cast<unsigned>(array::size(out_stream.buffer())),
			kvs::name_size(k_command_options), kvs::name(k_command_options),
			static_cast<unsigned>(array::size(out_stream.buffer())),
			array::begin(out_stream.buffer())
		);
		out_stream.clear();

		TOGO_ASSERTE(kvs::write(k_command, out_stream));
		TOGO_LOGF(
			"#### command (%u): `%.*s` <%.*s>\n",
			static_cast<unsigned>(array::size(out_stream.buffer())),
			kvs::name_size(k_command), kvs::name(k_command),
			static_cast<unsigned>(array::size(out_stream.buffer())),
			array::begin(out_stream.buffer())
		);
		out_stream.clear();
	}

	return 0;
}
