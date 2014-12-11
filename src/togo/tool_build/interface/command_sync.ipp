#line 2 "togo/tool_build/interface/command_sync.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

namespace togo {
namespace tool_build {

static bool sync_package(
	Interface& interface,
	PackageCompiler& pkg
) {
	StringRef const pkg_name = package_compiler::name(pkg);
	StringRef const pkg_path = package_compiler::path(pkg);
	TOGO_LOGF("syncing package '%.*s':\n", pkg_name.size, pkg_name.data);

	WorkingDirScope wd_scope{pkg_path};

	// Status:
	//   N: no change
	//   D: resource removed
	//   A: resource added
	//   I: ignored (path-parse failed or no resource handler for type)
	bool no_status_logs = true;
	#define TOGO_LOG_STATUS_(status, post)							\
		TOGO_LOGF(													\
			" %c  %.*s / %.*s " post "\n",							\
			status,													\
			pkg_name.size, pkg_name.data,							\
			path.size, path.data									\
		);															\
		no_status_logs = false

	{// Remove resources that have been deleted
	StringRef path;
	for (auto const& metadata : package_compiler::manifest(pkg)) {
		if (metadata.id == 0) {
			// Skip holes
			continue;
		}

		path = metadata.path;
		if (filesystem::is_file(path)) {
			TOGO_LOG_STATUS_('N', "");
		} else {
			TOGO_LOG_STATUS_('D', "");
			package_compiler::remove_resource(pkg, metadata.id);
		}
	}}

	{// Read new files in the package tree
	DirectoryReader dir_reader{};
	if (!directory_reader::open(dir_reader, ".", true, true)) {
		TOGO_LOG_ERRORF(
			"failed to open directory for package '%.*s': '%.*s'\n",
			pkg_name.size, pkg_name.data,
			pkg_path.size, pkg_path.data
		);
		return false;
	}

	ResourcePathParts pp;
	DirectoryEntry entry;
	StringRef path;
	while (directory_reader::read(dir_reader, entry, DirectoryEntry::Type::file)) {
		path = {
			entry.path.data + 2,
			static_cast<unsigned>(
				max(static_cast<signed>(entry.path.size) - 2, signed{0})
			)
		};
		if (!resource::parse_path(path, pp)) {
			TOGO_LOG_STATUS_('I', "(path parse failed)");
		} else if (!compiler_manager::has_compiler(interface._manager, pp.type_hash)) {
			TOGO_LOG_STATUS_('I', "(no compiler for type)");
		} else if (package_compiler::find_resource_id(pkg, pp, false) == 0) {
			TOGO_LOG_STATUS_('A', "");
			package_compiler::add_resource(pkg, path, pp);
		}
	}
	directory_reader::close(dir_reader);
	}
	#undef TOGO_LOG_STATUS_

	if (no_status_logs) {
		TOGO_LOG(" (no changes)\n\n");
	} else {
		TOGO_LOG("\n");
	}

	return package_compiler::write(pkg);
}

bool interface::command_sync(
	Interface& interface,
	StringRef const* package_names,
	unsigned num_package_names
) {
	TOGO_ASSERTE(package_names || num_package_names == 0);
	auto& packages = compiler_manager::packages(interface._manager);
	if (num_package_names > 0) {
		PackageCompiler* pkg;
		for (unsigned i = 0; i < num_package_names; ++i) {
			auto const& pkg_name = package_names[i];
			pkg = compiler_manager::get_package(
				const_cast<CompilerManager&>(interface._manager),
				resource::hash_package_name(pkg_name)
			);
			if (pkg) {
				if (!sync_package(interface, *pkg)) {
					return false;
				}
			} else {
				TOGO_LOG_ERRORF(
					"package not found: '%.*s'\n",
					pkg_name.size, pkg_name.data
				);
				return false;
			}
		}
	} else if (array::any(packages)) {
		for (auto* pkg : packages) {
			if (!sync_package(interface, *pkg)) {
				return false;
			}
		}
	} else {
		TOGO_LOG("no packages to sync\n");
	}
	return true;
}

bool interface::command_sync(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	if (kvs::any(k_command_options)) {
		TOGO_LOG("error: options unexpected\n");
		return false;
	}
	FixedArray<StringRef, 32> package_names{};
	for (KVS const& k_pkg_name : k_command) {
		if (!kvs::is_string(k_pkg_name) || kvs::string_size(k_pkg_name) == 0) {
			TOGO_LOG("error: expected non-empty string argument\n");
			return false;
		}
		fixed_array::push_back(package_names, kvs::string_ref(k_pkg_name));
	}
	return interface::command_sync(
		interface,
		fixed_array::begin(package_names),
		fixed_array::size(package_names)
	);
}

} // namespace tool_build
} // namespace togo
