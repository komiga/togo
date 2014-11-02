#line 2 "togo/tool_build/interface/command_help.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

namespace togo {
namespace tool_build {

bool interface::command_help(
	Interface const& /*interface*/,
	StringRef const& command_name
) {
	#define CASE_DESCRIBE_COMMAND(cmd, args, desc)				\
		case cmd ## _hash32:									\
		TOGO_LOG(cmd " " args "\n" desc);						\
		if (!do_all) {											\
			break;												\
		} else {												\
			TOGO_LOG("\n");										\
		}

	bool do_all = false;
	switch (hash::calc32(command_name)) {
		case ""_hash32:
			do_all = true;
			TOGO_LOG(
				TOGO_TOOL_BUILD_INFO_TEXT "\n"
				"\n"
				TOGO_TOOL_BUILD_USAGE_TEXT "\n"
				"  --project-path=<path>: specify project path\n"
				"  if this is not defined, the TOGO_PROJECT environment variable will be used\n"
				"\n"
			);

		CASE_DESCRIBE_COMMAND(
			"help", "[command_name]",
			"  prints tool_build help\n"
		);
		CASE_DESCRIBE_COMMAND(
			"list", "[-r|--resources || [<package_name> ...]]",
			"  lists packages and resources in the project\n"
			"\n"
			"  -r|--resources: print resources in all packages\n"
			"  [<package_name> ...]: print resources in selected packages (implicit -r)\n"
		);
		CASE_DESCRIBE_COMMAND(
			"create", "<package_name>",
			"  create package\n"
		);
		CASE_DESCRIBE_COMMAND(
			"sync", "[<package_name> ...]",
			"  sync packages with the filesystem\n"
			"  if no packages are specified, all packages are synced\n"
		);
		CASE_DESCRIBE_COMMAND(
			"compile", "[-f|--force] [--from=<package_name>] [<resource_path> ...]",
			"  compile resources\n"
			"  if no resources are specified, all are selected from the constraint\n"
			"\n"
			"  -f|--force: force building of selected resources that are already compiled\n"
			"  --from=<package_name>: only allow selection from the specified package;\n"
		);

	default:
		if (!do_all) {
			TOGO_LOGF(
				"error: command '%.*s' not recognized\n",
				command_name.size, command_name.data
			);
			return false;
		}
	}
	#undef CASE_DESCRIBE_COMMAND
	return true;
}

bool interface::command_help(
	Interface const& interface,
	KVS const& /*k_command_options*/,
	KVS const& k_command
) {
	if (kvs::empty(k_command)) {
		return command_help(interface, "");
	} else if (kvs::is_string(kvs::back(k_command))) {
		return command_help(interface, kvs::string_ref(kvs::back(k_command)));
	} else {
		TOGO_LOG("error: argument must be a string\n");
		return false;
	}
}

} // namespace tool_build
} // namespace togo
