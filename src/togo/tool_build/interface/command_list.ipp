#line 2 "togo/tool_build/interface/command_list.ipp"
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
	} else if (array::empty(pkg._metadata)) {
		TOGO_LOG("  no resources\n\n");
		return;
	}
	for (auto const& metadata : pkg._metadata) {
		TOGO_LOGF(
			"  %c [%08x %016lx] %.*s\n",
			metadata.last_compiled == 0 ? ' ' : 'C',
			metadata.type, metadata.name_hash,
			string::size(metadata.path),
			fixed_array::begin(metadata.path)
		);
	}
	TOGO_LOG("\n");
}

bool interface::command_list(
	Interface const& interface,
	bool const list_resources,
	StringRef const* const package_names,
	unsigned const num_package_names
) {
	TOGO_ASSERTE(package_names || num_package_names == 0);
	auto const& packages = compiler_manager::packages(interface._manager);
	if (num_package_names > 0) {
		PackageCompiler* pkg;
		for (unsigned i = 0; i < num_package_names; ++i) {
			auto const& pkg_name = package_names[i];
			pkg = compiler_manager::get_package(
				const_cast<CompilerManager&>(interface._manager),
				resource::hash_package_name(pkg_name)
			);
			if (pkg) {
				print_package(*pkg, true);
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
		}
	} else {
		TOGO_LOG("no packages\n");
	}
	return true;
}

bool interface::command_list(
	Interface const& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	bool opt_resources = false;
	for (KVS const& k_opt : k_command_options) {
		switch (kvs::name_hash(k_opt)) {
		case "-r"_kvs_name: // fall-through
		case "--resources"_kvs_name:
			if (!kvs::is_boolean(k_opt)) {
				TOGO_LOG("error: --resources: expected boolean value\n");
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
			TOGO_LOG("NB: takes no arguments with --resources\n");
		}
		return interface::command_list(interface, true, nullptr, 0);
	} else {
		FixedArray<StringRef, 32> package_names{};
		for (KVS const& k_pkg_name : k_command) {
			if (!kvs::is_string(k_pkg_name) || kvs::string_size(k_pkg_name) == 0) {
				TOGO_LOG("error: expected non-empty string argument\n");
				return false;
			}
			fixed_array::push_back(package_names, kvs::string_ref(k_pkg_name));
		}
		return interface::command_list(
			interface,
			false,
			fixed_array::begin(package_names),
			fixed_array::size(package_names)
		);
	}
}

} // namespace tool_build
} // namespace togo
