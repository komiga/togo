#line 2 "togo/tool_res_build/main.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/core/utility/args.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/tool_res_build/interface.hpp>

using namespace togo;
using namespace togo::tool_res_build;

signed main(signed argc, char* argv[]) {
	signed ec = 0;
	memory::init();

	{
		KVS k_options{};
		KVS k_command_options{};
		KVS k_command{};
		parse_args(k_options, k_command_options, k_command, argc, argv);

		Interface interface{};
		if (!interface::read_options(interface, k_options)) {
			ec = -1;
			goto l_exit;
		}
		interface::init(interface, "", true);
		interface::read_project(interface);

		if (!interface::run(interface, k_command_options, k_command)) {
			ec = -2;
			goto l_exit;
		}
	}

l_exit:
	memory::shutdown();
	return ec;
}
