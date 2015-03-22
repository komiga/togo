#line 2 "togo/tool_build/interface/command_pack.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

namespace togo {
namespace tool_build {

bool interface::command_pack(
	Interface& interface,
	bool const force,
	ArrayRef<StringRef const> package_names
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

	{// Build packages
	StringRef pkg_name;
	FixedArray<char, 80> build_path{};
	unsigned next = 1;
	signed print_size;
	for (auto* pkg : packages) {
		pkg_name = package_compiler::name(*pkg);
		TOGO_LOGF("PKG %.*s\n", pkg_name.size, pkg_name.data);
		if (!interface::command_compile(interface, force, pkg_name, nullptr, 0)) {
			return false;
		}

		if (package_compiler::needs_build(*pkg)) {
			print_size = std::snprintf(
				fixed_array::begin(build_path),
				fixed_array::capacity(build_path),
				"package/%.*s.package",
				pkg_name.size, pkg_name.data
			);
			TOGO_DEBUG_ASSERTE(print_size > 0);
			fixed_array::resize(build_path, print_size + 1);
			fixed_array::back(build_path) = '\0';
			TOGO_LOGF(
				"building '%.*s'...\n",
				string::size(build_path),
				fixed_array::begin(build_path)
			);
			{
				WorkingDirScope wd_scope{interface._project_path};
				if (!package_compiler::build(*pkg, build_path)) {
					return false;
				}
			}
			if (!package_compiler::write_properties(*pkg)) {
				return false;
			}
		}
		if (next++ < array::size(packages)) {
			TOGO_LOG("\n");
		}
	}}
	return true;
}

bool interface::command_pack(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	bool force = false;
	for (KVS const& k_opt : k_command_options) {
		switch (kvs::name_hash(k_opt)) {
		case "-f"_kvs_name:
			if (!kvs::is_boolean(k_opt)) {
				TOGO_LOG("error: -f: expected boolean value\n");
				return false;
			}
			force = kvs::boolean(k_opt);
			break;

		default:
			TOGO_LOGF(
				"error: option '%.*s' not recognized\n",
				kvs::name_size(k_opt), kvs::name(k_opt)
			);
			return false;
		}
	}

	Array<StringRef> names{memory::scratch_allocator()};
	for (KVS const& k_name : k_command) {
		if (!kvs::is_string(k_name) || kvs::string_size(k_name) == 0) {
			TOGO_LOG("error: expected non-empty string argument\n");
			return false;
		}
		array::push_back(names, kvs::string_ref(k_name));
	}
	return interface::command_pack(interface, force, names);
}

} // namespace tool_build
} // namespace togo
