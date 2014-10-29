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
	TOGO_LOGF("help: '%.*s'\n", command_name.size, command_name.data);
	return true;
}

bool interface::command_help(
	Interface const& interface,
	KVS const& /*k_command_options*/,
	KVS const& k_command
) {
	StringRef name{};
	if (kvs::any(k_command)) {
		TOGO_ASSERTE(kvs::is_string(k_command[0]));
		name = kvs::string_ref(k_command[0]);
	}
	return command_help(interface, name);
}

} // namespace tool_build
} // namespace togo
