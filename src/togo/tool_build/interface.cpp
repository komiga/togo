#line 2 "togo/tool_build/interface.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/string/string.hpp>
#include <togo/system/system.hpp>
#include <togo/filesystem/filesystem.hpp>
#include <togo/filesystem/directory_reader.hpp>
#include <togo/kvs/kvs.hpp>
#include <togo/resource/resource.hpp>
#include <togo/tool_build/resource_compiler.hpp>
#include <togo/tool_build/package_compiler.hpp>
#include <togo/tool_build/compiler_manager.hpp>
#include <togo/tool_build/generator_compiler.hpp>
#include <togo/tool_build/interface.hpp>

namespace togo {
namespace tool_build {

Interface::~Interface() {
	// TODO?
}

Interface::Interface()
	: _manager(memory::default_allocator())
	, _gfx_compiler(memory::default_allocator())
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

/// Read options.
///
/// This should only be called before init().
bool interface::read_options(
	Interface& interface,
	KVS const& k_options
) {
	TOGO_ASSERTE(kvs::is_node(k_options));
	for (KVS const& k_opt : k_options) {
		switch (kvs::name_hash(k_opt)) {
		case "--project-path"_kvs_name:
			if (!kvs::is_string(k_opt) || kvs::string_size(k_opt) == 0) {
				TOGO_LOG("error: --project-path expected a non-empty string\n");
				return false;
			}
			interface::set_project_path(interface, kvs::string_ref(k_opt));
			break;

		default:
			TOGO_LOGF(
				"error: option '%.*s' not recognized\n",
				kvs::name_size(k_opt), kvs::name(k_opt)
			);
			return false;
		}
	}
	return true;
}

/// Initialize.
///
/// This must be called for the project path to be assigned.
/// If project_path is empty, the TOGO_PROJECT environment variable
/// will be used.
/// If there is no project path, an assertion will fail.
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
		generator_compiler::register_standard(interface._gfx_compiler);
		resource_compiler::register_standard(interface._manager, interface._gfx_compiler);
	}
}

/// Add package by name.
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

/// Read project data.
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

/// Write project data.
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

/// Run tool with main() arguments.
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
		CMD_CASE(sync);
		CMD_CASE(compile);
		CMD_CASE(pack);
		CMD_CASE(compact);

	default:
		if (kvs::is_named(k_command)) {
			TOGO_LOGF(
				"error: command '%.*s' not recognized\n\n",
				kvs::name_size(k_command), kvs::name(k_command)
			);
		} else {
			TOGO_LOG("error: expected command\n\n");
		}
		TOGO_LOG(
			TOGO_TOOL_BUILD_USAGE_TEXT "\n"
			"use \"build help [command_name]\" for help\n"
		);
		success = false;
		break;
	}
	#undef CMD_CASE

	if (success) {
		interface::write_project(interface);
		TOGO_ASSERTE(compiler_manager::write_packages(interface._manager));
	}

	return success;
}

} // namespace tool_build
} // namespace togo

#include <togo/tool_build/interface/command_help.ipp>
#include <togo/tool_build/interface/command_list.ipp>
#include <togo/tool_build/interface/command_create.ipp>
#include <togo/tool_build/interface/command_sync.ipp>
#include <togo/tool_build/interface/command_compile.ipp>
#include <togo/tool_build/interface/command_pack.ipp>
#include <togo/tool_build/interface/command_compact.ipp>
