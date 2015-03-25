#line 2 "togo/tool_build/compiler_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/tool_build/config.hpp>
#include <togo/tool_build/types.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/resource/resource.hpp>
#include <togo/tool_build/resource_compiler.hpp>
#include <togo/tool_build/package_compiler.hpp>
#include <togo/tool_build/compiler_manager.hpp>

namespace togo {
namespace tool_build {

CompilerManager::~CompilerManager() {
	compiler_manager::clear_packages(*this);
}

CompilerManager::CompilerManager(Allocator& allocator)
	: _compilers(allocator)
	, _packages(allocator)
{
	hash_map::reserve(_compilers, 16);
	array::reserve(_packages, 8);
}

/// Add a resource compiler.
///
/// An assertion will fail if the resource type has already been
/// registered.
void compiler_manager::register_compiler(
	CompilerManager& cm,
	ResourceCompiler const& compiler
) {
	TOGO_ASSERT(
		compiler.func_compile,
		"func_compile must be assigned in compiler"
	);
	TOGO_ASSERT(
		!hash_map::has(cm._compilers, compiler.type),
		"type has already been registered"
	);
	hash_map::push(cm._compilers, compiler.type, compiler);
}

/// Whether there is a compiler for type registered.
bool compiler_manager::has_compiler(
	CompilerManager const& cm,
	ResourceType const type
) {
	return hash_map::has(cm._compilers, type);
}

/// Find resource compiler.
ResourceCompiler const* compiler_manager::find_compiler(
	CompilerManager const& cm,
	ResourceType const type
) {
	return hash_map::find(cm._compilers, type);
}

/// Whether a package with name_hash exists.
bool compiler_manager::has_package(
	CompilerManager const& cm,
	ResourcePackageNameHash const name_hash
) {
	for (auto const* pkg : cm._packages) {
		if (pkg->_name_hash == name_hash) {
			return true;
		}
	}
	return false;
}

/// Find package by name hash.
PackageCompiler* compiler_manager::find_package(
	CompilerManager& cm,
	ResourcePackageNameHash const name_hash
) {
	for (auto* pkg : cm._packages) {
		if (pkg->_name_hash == name_hash) {
			return pkg;
		}
	}
	return nullptr;
}

/// Find packages by names.
///
/// packages elements will be nullptr when lookup for its name misses.
/// Returns false if a lookup fails.
bool compiler_manager::find_packages(
	CompilerManager& cm,
	Array<PackageCompiler*>& packages,
	ArrayRef<StringRef const> const names
) {
	array::resize(packages, names.size());
	bool lookup_failed = false;
	for (unsigned i = 0; i < names.size(); ++i) {
		StringRef const& pkg_name = names[i];
		packages[i] = compiler_manager::find_package(
			cm, resource::hash_package_name(pkg_name)
		);
		if (!packages[i]) {
			TOGO_LOG_ERRORF("package '%.*s' not found\n", pkg_name.size, pkg_name.data);
			lookup_failed = true;
		}
	}
	return !lookup_failed;
}

/// Add package.
///
/// An assertion will fail if the package is already in the manager.
/// Returns package name hash.
ResourcePackageNameHash compiler_manager::add_package(
	CompilerManager& cm,
	StringRef const& path
) {
	// NB: PackageCompiler ctor trims path
	Allocator& allocator = *cm._packages._allocator;
	PackageCompiler* const pkg = TOGO_CONSTRUCT(allocator,
		PackageCompiler, path, allocator
	);
	for (auto const* it_pkg : cm._packages) {
		TOGO_ASSERTF(
			!string::compare_equal(it_pkg->_path, pkg->_path),
			"package at '%.*s' has already been added",
			string::size(it_pkg->_path), fixed_array::begin(it_pkg->_path)
		);
	}
	array::push_back(cm._packages, pkg);
	package_compiler::read(*pkg);
	return package_compiler::name_hash(*pkg);
}

/// Remove package.
///
/// An assertion will fail if the package does not exist.
void compiler_manager::remove_package(
	CompilerManager& cm,
	ResourcePackageNameHash const name_hash
) {
	Allocator& allocator = *cm._packages._allocator;
	for (unsigned i = 0; i < array::size(cm._packages); ++i) {
		auto* const pkg = cm._packages[i];
		if (pkg->_name_hash == name_hash) {
			// TODO: Unload active resources from the package
			TOGO_DESTROY(allocator, pkg);
			array::remove(cm._packages, i);
			return;
		}
	}
	TOGO_ASSERT(false, "package not found");
}

/// Remove all packages.
void compiler_manager::clear_packages(
	CompilerManager& cm
) {
	Allocator& allocator = *cm._packages._allocator;
	for (auto* pkg : cm._packages) {
		TOGO_DESTROY(allocator, pkg);
	}
	array::clear(cm._packages);
}

/// Write all packages that are modified.
bool compiler_manager::write_packages(
	CompilerManager& cm
) {
	for (auto* pkg : cm._packages) {
		if (!package_compiler::write(*pkg)) {
			return false;
		}
	}
	return true;
}

/// Whether a resource with identity exists.
bool compiler_manager::has_resource(
	CompilerManager const& cm,
	ResourceType const type,
	ResourceNameHash const name_hash,
	ResourceTagsHash const tags_hash IGEN_DEFAULT(RES_TAGS_NULL)
) {
	for (auto const* pkg : cm._packages) {
		if (package_compiler::find_resource_id(
			*pkg, type, name_hash, tags_hash, false
		) != 0) {
			return true;
		}
	}
	return false;
}

/// Find lookup node by resource name.
PackageCompiler::LookupNode* compiler_manager::find_node(
	CompilerManager& cm,
	ResourceNameHash const name_hash,
	PackageCompiler*& package
) {
	PackageCompiler::LookupNode* node;
	for (
		auto* it_pkg = array::end(cm._packages) - 1;
		it_pkg >= array::begin(cm._packages);
		--it_pkg
	) {
		node = package_compiler::find_node(**it_pkg, name_hash);
		if (node) {
			package = *it_pkg;
			return node;
		}
	}
	package = nullptr;
	return nullptr;
}

} // namespace tool_build
} // namespace togo
