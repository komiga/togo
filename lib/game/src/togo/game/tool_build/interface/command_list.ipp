#line 2 "togo/game/tool_build/interface/command_list.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

namespace togo {
namespace tool_build {

static void print_package(
	PackageCompiler const& pkg,
	bool const list_resources
) {
	auto const pkg_name = package_compiler::name(pkg);
	TOGO_LOGF(
		"[%08x] %.*s\n",
		package_compiler::name_hash(pkg),
		pkg_name.size, pkg_name.data
	);
	if (!list_resources) {
		return;
	} else if (array::empty(package_compiler::manifest(pkg))) {
		TOGO_LOG("  no resources\n");
		return;
	}
	for (auto const& metadata : package_compiler::manifest(pkg)) {
		TOGO_LOGF(
			"  %c [%08x %016lx %-4u] %.*s\n",
			metadata.last_compiled == 0 ? ' ' : 'C',
			metadata.type, metadata.name_hash, metadata.id,
			string::size(metadata.path),
			fixed_array::begin(metadata.path)
		);
	}
}

/// Run list command.
///
/// Lists packages in the project or resources in packages.
bool interface::command_list(
	Interface const& interface,
	bool const list_resources,
	ArrayRef<StringRef const> const package_names
) {
	auto const& packages = compiler_manager::packages(interface._manager);
	unsigned next = 1;
	if (package_names.size() > 0) {
		for (auto const& pkg_name : package_names) {
			auto const* pkg = compiler_manager::find_package(
				const_cast<CompilerManager&>(interface._manager),
				resource::hash_package_name(pkg_name)
			);
			if (pkg) {
				print_package(*pkg, true);
				if (next++ < package_names.size()) {
					TOGO_LOG("\n");
				}
			} else {
				TOGO_LOGF(
					"package not found: '%.*s'\n",
					pkg_name.size, pkg_name.data
				);
			}
		}
	} else if (array::any(packages)) {
		for (auto const* pkg : packages) {
			print_package(*pkg, list_resources);
			if (list_resources && next++ < array::size(packages)) {
				TOGO_LOG("\n");
			}
		}
	} else {
		TOGO_LOG("no packages\n");
	}
	return true;
}

/// Run list command with KVS.
///
/// Specification:
/// @verbatim list [-r || [<package_name> ...]] @endverbatim
bool interface::command_list(
	Interface const& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	bool opt_resources = false;
	for (KVS const& k_opt : k_command_options) {
		switch (kvs::name_hash(k_opt)) {
		case "-r"_kvs_name:
			if (!kvs::is_boolean(k_opt)) {
				TOGO_LOG("error: -r: expected boolean value\n");
				return false;
			}
			opt_resources = kvs::boolean(k_opt);
			break;

		default:
			TOGO_LOGF(
				"error: option '%.*s' not recognized\n",
				kvs::name_size(k_opt), kvs::name(k_opt)
			);
			return false;
		}
	}
	if (opt_resources) {
		if (kvs::any(k_command)) {
			TOGO_LOG("NB: takes no arguments with -r\n");
		}
		return interface::command_list(interface, true, null_ref_tag{});
	} else {
		Array<StringRef> package_names{memory::scratch_allocator()};
		for (KVS const& k_pkg_name : k_command) {
			if (!kvs::is_string(k_pkg_name) || kvs::string_size(k_pkg_name) == 0) {
				TOGO_LOG("error: expected non-empty string argument\n");
				return false;
			}
			array::push_back(package_names, kvs::string_ref(k_pkg_name));
		}
		return interface::command_list(interface, false, package_names);
	}
}

} // namespace tool_build
} // namespace togo
