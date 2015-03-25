#line 2 "togo/tool_build/interface/command_compact.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

namespace togo {
namespace tool_build {

/// Run compact command.
///
/// Removes empty metadata entries.
/// If no packages are specified, all packages are selected.
bool interface::command_compact(
	Interface& interface,
	ArrayRef<StringRef const> const package_names
) {
	Array<PackageCompiler*> packages{memory::scratch_allocator()};
	if (package_names.size() == 0) {
		array::copy(packages, compiler_manager::packages(interface._manager));
	} else if (!compiler_manager::find_packages(
		interface._manager, packages, package_names
	)) {
		return false;
	}

	if (array::empty(packages)) {
		TOGO_LOG("(no packages to build)\n");
		return true;
	}

	{// Compact packages
	StringRef pkg_name;
	unsigned next = 1;
	for (auto* pkg : packages) {
		pkg_name = package_compiler::name(*pkg);
		TOGO_LOGF("compacting package '%.*s'\n", pkg_name.size, pkg_name.data);
		if (package_compiler::compact(*pkg)) {
			TOGO_LOG(" (modified)\n");
			if (!package_compiler::write(*pkg)) {
				return false;
			}
		}
		if (next++ < array::size(packages)) {
			TOGO_LOG("\n");
		}
	}}
	return true;
}

/// Run compact command with KVS.
///
/// Specification:
/// @verbatim compact [<package_name> ...] @endverbatim
bool interface::command_compact(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	if (kvs::any(k_command_options)) {
		TOGO_LOG("error: options unexpected\n");
		return false;
	}

	Array<StringRef> names{memory::scratch_allocator()};
	for (KVS const& k_name : k_command) {
		if (!kvs::is_string(k_name) || kvs::string_size(k_name) == 0) {
			TOGO_LOG("error: expected non-empty string argument\n");
			return false;
		}
		array::push_back(names, kvs::string_ref(k_name));
	}
	return interface::command_compact(interface, names);
}

} // namespace tool_build
} // namespace togo
