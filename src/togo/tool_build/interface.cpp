#line 2 "togo/tool_build/interface.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/memory.hpp>
#include <togo/array.hpp>
#include <togo/string.hpp>
#include <togo/system.hpp>
#include <togo/filesystem.hpp>
#include <togo/kvs.hpp>
#include <togo/resource.hpp>
#include <togo/tool_build/package_compiler.hpp>
#include <togo/tool_build/compiler_manager.hpp>
#include <togo/tool_build/interface.hpp>

namespace togo {
namespace tool_build {

Interface::~Interface() {
	// TODO?
}

Interface::Interface()
	: _manager(memory::default_allocator())
	, _project_path()
{}

namespace interface {

inline static void check_project_path(
	Interface const& interface
) {
	TOGO_ASSERT(
		string::size(interface._project_path) != 0,
		"project path not set"
	);
	TOGO_ASSERT(
		filesystem::is_directory(interface._project_path),
		"project path does not exist"
	);
}

static void set_project_path(
	Interface& interface,
	StringRef path
) {
	if (path.empty()) {
		path = system::environment_variable("TOGO_PROJECT");
	}
	string::copy(interface._project_path, path);
	string::trim_trailing_slashes(interface._project_path);
}

} // namespace interface

bool interface::read_options(
	Interface& interface,
	KVS const& k_options
) {
	TOGO_ASSERTE(kvs::is_node(k_options));
	for (KVS const& k_opt : k_options) {
		switch (kvs::name_hash(k_opt)) {
		case "--project-path"_kvs_name:
			if (!kvs::is_string(k_opt) || kvs::string_size(k_opt) == 0) {
				TOGO_LOG_ERROR("--project-path: expected a non-empty string\n");
				return false;
			}
			interface::set_project_path(interface, kvs::string_ref(k_opt));
			break;

		default:
			TOGO_LOG_ERRORF(
				"option '%.*s' not recognized\n",
				kvs::name_size(k_opt), kvs::name(k_opt)
			);
			return false;
		}
	}
	return true;
}

void interface::init(
	Interface& interface,
	StringRef const& project_path,
	bool const register_standard_compilers
) {
	if (project_path.any() || string::size(interface._project_path) == 0) {
		interface::set_project_path(interface, project_path);
	}
	interface::check_project_path(interface);
	if (register_standard_compilers) {
		compiler_manager::register_standard_compilers(interface._manager);
	}
}

void interface::add_package(
	Interface& interface,
	StringRef const& name
) {
	TOGO_ASSERTE(name.any());
	interface::check_project_path(interface);
	ResourcePackageNameHash const name_hash = resource::hash_package_name(name);
	TOGO_ASSERTF(
		!compiler_manager::has_package(interface._manager, name_hash),
		"package '%.*s' already exists in the project",
		name.size, name.data
	);

	FixedArray<char, 256> path{};
	string::append(path, interface._project_path);
	string::append(path, "/package/");
	string::append(path, name);
	compiler_manager::add_package(interface._manager, path);
}

void interface::read_project(
	Interface& interface
) {
	interface::check_project_path(interface);
	compiler_manager::clear_packages(interface._manager);

	WorkingDirScope wd_scope{interface._project_path};

	{// Read package list
	KVS k_root{};
	TOGO_ASSERTE(kvs::read_file(k_root, ".project/packages"));
	KVS const* const k_packages = kvs::find(k_root, "packages");
	TOGO_ASSERT(
		k_packages && kvs::is_array(*k_packages),
		".project/packages: 'packages' must be an array"
	);
	for (KVS const& k_name : *k_packages) {
		TOGO_ASSERT(
			kvs::is_string(k_name),
			".project/packages: 'packages' elements must be strings"
		);
		interface::add_package(interface, kvs::string_ref(k_name));
	}}
}

void interface::write_project(
	Interface& interface
) {
	interface::check_project_path(interface);

	WorkingDirScope wd_scope{interface._project_path};

	{// Write package list
	KVS k_root{KVSType::node};
	KVS& k_packages = kvs::push_back(k_root, KVS{"packages", KVSType::array});
	auto const& packages = compiler_manager::packages(interface._manager);
	for (auto const* pkg : packages) {
		kvs::push_back(k_packages, KVS{package_compiler::name(*pkg)});
	}
	TOGO_ASSERTE(kvs::write_file(k_root, ".project/packages"));
	}
}

bool interface::run(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	TOGO_ASSERTE(
		kvs::is_node(k_command_options) &&
		kvs::is_node(k_command)
	);
	interface::check_project_path(interface);

	if (!kvs::is_named(k_command)) {
		TOGO_LOG_ERROR("expected command\n");
		return false;
	}

	#define CMD_CASE(cmd)										\
		case #cmd ## _kvs_name:									\
		success = interface::command_ ## cmd(					\
			interface, k_command_options, k_command				\
		);														\
		break

	bool success = false;
	switch (kvs::name_hash(k_command)) {
		CMD_CASE(help);
		CMD_CASE(list);
		CMD_CASE(create);

	default:
		TOGO_LOG_ERRORF(
			"command '%.*s' not recognized\n",
			kvs::name_size(k_command), kvs::name(k_command)
		);
		success = false;
		break;
	}
	#undef CMD_CASE

	if (success) {
		interface::write_project(interface);
	}
	return success;
}

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

static void print_package(
	PackageCompiler const& pkg,
	bool const list_resources
) {
	auto const pkg_name = package_compiler::name(pkg);
	TOGO_LOGF(
		"[%08x] '%.*s'\n",
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
			"  %c [%08x %016lx] '%.*s'\n",
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
				TOGO_LOG_ERROR("list: --resources: expected boolean value\n");
				return false;
			}
			opt_resources = kvs::boolean(k_opt);
			break;

		default:
			TOGO_LOG_ERRORF(
				"list: option '%.*s' not recognized\n",
				kvs::name_size(k_opt), kvs::name(k_opt)
			);
			return false;
		}
	}
	if (opt_resources) {
		if (kvs::any(k_command)) {
			TOGO_LOG("list: NB: takes no arguments with --resources\n");
		}
		return interface::command_list(interface, true, nullptr, 0);
	} else {
		FixedArray<StringRef, 32> package_names{};
		for (KVS const& k_pkg_name : k_command) {
			if (!kvs::is_string(k_pkg_name) || kvs::string_size(k_pkg_name) == 0) {
				TOGO_LOG_ERROR("list: expected non-empty string argument\n");
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

bool interface::command_create(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
) {
	if (kvs::any(k_command_options)) {
		TOGO_LOG_ERROR("create: options unexpected\n");
		return false;
	} else if (kvs::empty(k_command)) {
		TOGO_LOG_ERROR("create: <name> expected\n");
		return false;
	} else if (kvs::size(k_command) > 1) {
		TOGO_LOG_ERROR("create: too many arguments\n");
		return false;
	}

	KVS const& k_name = k_command[0];
	TOGO_ASSERTE(kvs::is_string(k_name));
	return interface::command_create(interface, kvs::string_ref(k_name));
}

} // namespace tool_build
} // namespace togo
