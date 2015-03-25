#line 2 "togo/tool_build/interface/command_create.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

namespace togo {
namespace tool_build {

/// Run create command.
///
/// Creates package.
bool interface::command_create(
	Interface& interface,
	StringRef const& name
) {
	interface::check_project_path(interface);
	if (name.empty()) {
		TOGO_LOG_ERROR("package name must not be empty\n");
		return false;
	}
	ResourcePackageNameHash const name_hash = resource::hash_package_name(name);
	if (compiler_manager::has_package(interface._manager, name_hash)) {
		TOGO_LOG_ERRORF(
			"package '%.*s' already exists in the project\n",
			name.size, name.data
		);
		return false;
	}

	FixedArray<char, 256> path{};
	string::append(path, interface._project_path);
	string::append(path, "/package/");
	string::append(path, name);
	if (!package_compiler::create_stub(path, name)) {
		return false;
	}
	compiler_manager::add_package(interface._manager, path);
	return true;
}

/// Run create command with KVS.
///
/// Specification:
/// @verbatim create <package_name> @endverbatim
bool interface::command_create(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	if (kvs::any(k_command_options)) {
		TOGO_LOG("error: options unexpected\n");
		return false;
	} else if (kvs::empty(k_command)) {
		TOGO_LOG("error: <name> expected\n");
		return false;
	} else if (kvs::size(k_command) > 1) {
		TOGO_LOG("error: too many arguments\n");
		return false;
	}

	KVS const& k_name = k_command[0];
	TOGO_ASSERTE(kvs::is_string(k_name));
	return interface::command_create(interface, kvs::string_ref(k_name));
}

} // namespace tool_build
} // namespace togo
