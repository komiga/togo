#line 2 "togo/tool_build/interface.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief tool_build interface.
@ingroup tool_build_interface
*/

#pragma once

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/string/types.hpp>
#include <togo/kvs/types.hpp>

namespace togo {
namespace tool_build {
namespace interface {

/**
	@addtogroup tool_build_interface
	@{
*/

/// Get project path.
inline StringRef project_path(
	Interface const& interface
) {
	return {interface._project_path};
}

/// Read options.
///
/// This should only be called before init().
bool read_options(
	Interface& interface,
	KVS const& k_options
);

/// Initialize interface.
///
/// This must be called for the project path to be assigned.
/// If project_path is empty, the TOGO_PROJECT environment variable
/// will be used.
/// If there is no project path, an assertion will fail.
void init(
	Interface& interface,
	StringRef const& project_path,
	bool const register_standard_compilers
);

/// Add package by name.
void add_package(
	Interface& interface,
	StringRef const& name
);

/// Read project data.
void read_project(
	Interface& interface
);

/// Write project data.
void write_project(
	Interface& interface
);

/// Run tool with main() arguments.
bool run(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
);

/// Run help command.
///
/// Prints command help information.
bool command_help(
	Interface const& interface,
	StringRef const& command_name
);

/// Run help command with KVS.
///
/// Specification:
/// @verbatim help [command_name] @endverbatim
bool command_help(
	Interface const& interface,
	KVS const& k_command_options,
	KVS const& k_command
);

/// Run list command.
///
/// Lists packages in the project or resources in packages.
bool command_list(
	Interface const& interface,
	bool list_resources,
	StringRef const* package_names,
	unsigned num_package_names
);

/// Run list command with KVS.
///
/// Specification:
/// @verbatim list [-r || [<package_name> ...]] @endverbatim
bool command_list(
	Interface const& interface,
	KVS const& k_command_options,
	KVS const& k_command
);

/// Run create command.
///
/// Creates package.
bool command_create(
	Interface& interface,
	StringRef const& name
);

/// Run create command with KVS.
///
/// Specification:
/// @verbatim create <package_name> @endverbatim
bool command_create(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
);

/// Run sync command.
///
/// Updates package manifests to match filesystem.
/// If no packages are specified, all packages are synced.
bool command_sync(
	Interface& interface,
	StringRef const* package_names,
	unsigned num_package_names
);

/// Run sync command with KVS.
///
/// Specification:
/// @verbatim sync [<package_name> ...] @endverbatim
bool command_sync(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
);

/// Run compile command.
///
/// Builds resources.
/// If a package is specified, only resources within that package can
/// be selected.
/// If no resources are specified, all unbuilt resources are compiled.
/// If force is true, all selected resources are built regardless of
/// their source-compilation parity.
bool command_compile(
	Interface& interface,
	bool const force,
	StringRef const& from_package_name,
	StringRef const* paths,
	unsigned num_paths
);

/// Run compile command with KVS.
///
/// Specification:
/// @verbatim compile [-f] [--from=<package_name>] [<resource_path> ...] @endverbatim
bool command_compile(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
);

/// Run pack command.
///
/// Builds packages.
/// If no packages are specified, all packages are selected.
/// If force is true, all selected packages are built regardless of
/// their source-compilation parity.
bool command_pack(
	Interface& interface,
	bool const force,
	ArrayRef<StringRef const> package_names
);

/// Run pack command with KVS.
///
/// Specification:
/// @verbatim pack [-f] [<package_name> ...] @endverbatim
bool command_pack(
	Interface& interface,
	KVS const& k_command_options,
	KVS const& k_command
);

/** @} */ // end of doc-group tool_build_interface

} // namespace interface
} // namespace tool_build
} // namespace togo
