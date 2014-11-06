#line 2 "togo/tool_build/interface/command_compile.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

namespace togo {
namespace tool_build {

static bool compile_resource(
	Interface& interface,
	PackageCompiler& pkg,
	ResourceCompilerMetadata& metadata
) {
	FixedArray<char, 24> output_path{};
	resource_metadata::compiled_path(metadata, output_path);

	// Open streams
	FileReader in_stream{};
	FileWriter out_stream{};
	if (!in_stream.open(metadata.path)) {
		TOGO_LOG_ERROR("failed to open source file\n");
		return false;
	}
	if (!out_stream.open(output_path, false)) {
		TOGO_LOG_ERRORF(
			"failed to open output file: '%.*s'\n",
			string::size(output_path), fixed_array::begin(output_path)
		);
		in_stream.close();
		return false;
	}

	{// Compile
	auto const* const compiler = compiler_manager::get_compiler(
		interface._manager,
		metadata.type
	);
	if (!compiler) {
		TOGO_LOG_ERROR("no compiler for type\n");
		return false;
	}
	bool const success = compiler->func_compile(
		interface._manager,
		pkg, metadata,
		in_stream, out_stream
	);
	in_stream.close();
	out_stream.close();
	if (success) {
		metadata.data_format_version = compiler->format_version;
		metadata.last_compiled = filesystem::time_last_modified(output_path);
	} else {
		metadata.data_format_version = 0;
		metadata.last_compiled = 0;
		TOGO_LOG_ERROR("failed to compile\n");
		return false;
	}}

	package_compiler::set_manifest_modified(pkg, true);
	return true;
}

static bool add_resource(
	HashMap<ResourcePackageNameHash, Array<u32>*>& groups,
	PackageCompiler const* const pkg,
	u32 const id,
	bool const force
) {
	TOGO_ASSERTE(pkg);
	if (id == 0) {
		// Skip holes
		return false;
	}

	auto const& metadata = pkg->_manifest[id - 1];
	StringRef const pkg_name{package_compiler::name(*pkg)};
	StringRef const path{metadata.path};
	u64 const modified = filesystem::time_last_modified(path);
	if (modified == 0) {
		TOGO_LOG_ERRORF(
			"resource file has vanished: %.*s / %.*s\n",
			pkg_name.size, pkg_name.data,
			path.size, path.data
		);
		return false;
	} else if (
		!force &&
		metadata.last_compiled != 0 &&
		metadata.last_compiled >= modified
	) {
		// Resource doesn't need to be compiled
		return false;
	}

	auto const pkg_name_hash = package_compiler::name_hash(*pkg);
	auto** res_list_g = hash_map::get(groups, pkg_name_hash);
	auto* res_list = res_list_g ? *res_list_g : nullptr;
	if (!res_list) {
		Allocator& allocator = *groups._head._allocator;
		res_list = hash_map::push(
			groups, pkg_name_hash,
			TOGO_CONSTRUCT(allocator, Array<u32>, allocator)
		);
		array::reserve(*res_list, 16);
	}
	array::push_back(*res_list, id);
	return true;
}

static void add_node(
	HashMap<ResourcePackageNameHash, Array<u32>*>& groups,
	PackageCompiler const* const pkg,
	PackageCompiler::LookupNode const* node,
	bool const force
) {
	TOGO_ASSERTE(pkg && node);
	StringRef const pkg_name = package_compiler::name(*pkg);
	for (; node != nullptr; node = hash_map::get_next(pkg->_lookup, node)) {
		if (!add_resource(groups, pkg, node->value, force)) {
			StringRef const path{pkg->_manifest[node->value - 1].path};
			TOGO_LOGF(
				" N  %.*s / %.*s\n",
				pkg_name.size, pkg_name.data,
				path.size, path.data
			);
		}
	}
}

// Status:
//   N: no compile needed
//   C: compiling

bool interface::command_compile(
	Interface& interface,
	bool const force,
	StringRef const& from_package_name,
	StringRef const* paths,
	unsigned num_paths
) {
	TOGO_ASSERTE(paths || num_paths == 0);

	auto* const from_package = compiler_manager::get_package(
		interface._manager,
		resource::hash_package_name(from_package_name)
	);
	if (from_package_name.any() && !from_package) {
		TOGO_LOG_ERRORF(
			"package '%.*s' not found\n",
			from_package_name.size, from_package_name.data
		);
		return false;
	}

	bool success = true;
	HashMap<ResourcePackageNameHash, Array<u32>*> groups{
		memory::scratch_allocator()
	};
	hash_map::reserve(
		groups,
		array::size(compiler_manager::packages(interface._manager))
	);

	{// Add resources
	PackageCompiler* pkg;
	PackageCompiler::LookupNode* node;
	if (num_paths > 0) {
		ResourcePackageNameHash wd_current_pkg = PKG_NAME_NULL;
		FixedArray<char, 256> wd_prev{};
		TOGO_ASSERTE(filesystem::working_dir(wd_prev));
		StringRef path{};
		ResourcePathParts pp{};
		for (unsigned i = 0; i < num_paths; ++i) {
			path = paths[i];

			// TODO: Typeless parse
			if (!resource::parse_path(path, pp)) {
				success = false;
				TOGO_LOG_ERRORF(
					"failed to parse path: '%.*s'\n",
					path.size, path.data
				);
				continue;
			}

			pkg = from_package;
			if (from_package) {
				node = package_compiler::get_node(
					*from_package, pp.name_hash
				);
			} else {
				node = compiler_manager::get_node(
					interface._manager, pp.name_hash, pkg
				);
			}
			if (node) {
				if (wd_current_pkg != package_compiler::name_hash(*pkg)) {
					if (wd_current_pkg != PKG_NAME_NULL) {
						TOGO_ASSERTE(filesystem::set_working_dir(wd_prev));
					}
					TOGO_ASSERTE(filesystem::set_working_dir(
						package_compiler::path(*pkg)
					));
					wd_current_pkg = package_compiler::name_hash(*pkg);
				}
				add_node(groups, pkg, node, force);
			} else {
				success = false;
				TOGO_LOG_ERRORF(
					"resource does not exist: '%.*s'\n",
					path.size, path.data
				);
			}
		}
		if (wd_current_pkg != PKG_NAME_NULL) {
			TOGO_ASSERTE(filesystem::set_working_dir(wd_prev));
		}
	} else if (from_package) {
		WorkingDirScope wd_scope{package_compiler::path(*from_package)};
		for (auto const& metadata : package_compiler::manifest(*from_package)) {
			add_resource(groups, from_package, metadata.id, force);
		}
	} else {
		// All resources
		for (auto const* pkg : compiler_manager::packages(interface._manager)) {
			WorkingDirScope wd_scope{package_compiler::path(*pkg)};
		for (auto const& metadata : package_compiler::manifest(*pkg)) {
			add_resource(groups, pkg, metadata.id, force);
		}}
	}}

	if (!success) {
		goto l_exit;
	} else if (hash_map::empty(groups)) {
		TOGO_LOG("no resources to build\n");
		goto l_exit;
	}

	// TODO: Write package data after compiling
	{// Compile resources
	PackageCompiler* pkg;
	Array<u32> const* res_list;
	StringRef pkg_name{};
	StringRef path{};
	for (auto const& node : groups) {
		pkg = compiler_manager::get_package(interface._manager, node.key);
		TOGO_ASSERTE(pkg);
		res_list = node.value;
		pkg_name = package_compiler::name(*pkg);
		WorkingDirScope wd_scope{package_compiler::path(*pkg)};
	for (u32 id : *res_list) {
		auto& metadata = pkg->_manifest[id - 1];
		path = StringRef{metadata.path};
		TOGO_LOGF(
			"  C %.*s / %.*s\n",
			pkg_name.size, pkg_name.data,
			path.size, path.data
		);
		if (!compile_resource(interface, *pkg, metadata)) {
			success = false;
			goto l_exit;
		}
	}
		if (&node + 1 != hash_map::end(groups)) {
			TOGO_LOG("\n");
		}
	}}

l_exit:
	for (auto& node : groups) {
		TOGO_DESTROY(*groups._head._allocator, node.value);
		node.value = nullptr;
	}
	return success;
}

bool interface::command_compile(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	bool force = false;
	StringRef from_package_name{};
	for (KVS const& k_opt : k_command_options) {
		switch (kvs::name_hash(k_opt)) {
		case "-f"_kvs_name:
			if (!kvs::is_boolean(k_opt)) {
				TOGO_LOG("error: -f: expected boolean value\n");
				return false;
			}
			force = kvs::boolean(k_opt);
			break;

		case "--from"_kvs_name:
			if (!kvs::is_string(k_opt)) {
				TOGO_LOG("error: --from: expected string value\n");
				return false;
			}
			from_package_name = kvs::string_ref(k_opt);
			break;

		default:
			TOGO_LOGF(
				"error: option '%.*s' not recognized\n",
				kvs::name_size(k_opt), kvs::name(k_opt)
			);
			return false;
		}
	}

	FixedArray<StringRef, 16> paths{};
	for (KVS const& k_res : k_command) {
		if (!kvs::is_string(k_res) || kvs::string_size(k_res) == 0) {
			TOGO_LOG("error: expected non-empty string argument\n");
			return false;
		}
		fixed_array::push_back(paths, kvs::string_ref(k_res));
	}

	return interface::command_compile(
		interface,
		force,
		from_package_name,
		fixed_array::begin(paths),
		fixed_array::size(paths)
	);
}

} // namespace tool_build
} // namespace togo
