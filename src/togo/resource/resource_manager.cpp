#line 2 "togo/resource/resource_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/log/log.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource.hpp>
#include <togo/resource/resource_package.hpp>
#include <togo/resource/resource_manager.hpp>

namespace togo {

namespace resource_manager {

static bool find_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash,
	ResourcePackage*& package,
	ResourcePackage::LookupNode*& node
) {
	// TODO: Tag filter
	ResourcePackage::LookupNode* it_node;
	for (
		auto* it = array::end(rm._packages) - 1;
		it >= array::begin(rm._packages);
		--it
	) {
		it_node = resource_package::find_resource(**it, type, name_hash);
		if (it_node) {
			package = *it;
			node = it_node;
			return true;
		}
	}
	return false;
}

} // namespace resource_manager

ResourceManager::~ResourceManager() {
	resource_manager::clear_packages(*this);
}

ResourceManager::ResourceManager(
	StringRef const base_path,
	Allocator& allocator
)
	: _handlers(allocator)
	, _resources(allocator)
	, _packages(allocator)
	, _base_path()
{
	hash_map::reserve(_handlers, 16);
	string::copy(_base_path, base_path);
	string::ensure_trailing_slash(_base_path);
}

void resource_manager::register_handler(
	ResourceManager& rm,
	ResourceHandler const& handler,
	void* type_data
) {
	TOGO_ASSERT(
		handler.func_load && handler.func_unload,
		"func_load and func_unload must be assigned in handler"
	);
	TOGO_ASSERT(
		!hash_map::has(rm._handlers, handler.type),
		"type has already been registered"
	);
	hash_map::push(rm._handlers, handler.type, {handler, type_data});
}

bool resource_manager::has_handler(
	ResourceManager const& rm,
	ResourceType type
) {
	return hash_map::has(rm._handlers, type);
}

ResourcePackageNameHash resource_manager::add_package(
	ResourceManager& rm,
	StringRef const& name
) {
	FixedArray<char, 256> path{};
	string::copy(path, rm._base_path);
	string::append(path, name);
	string::append(path, ".package");

	return resource_manager::add_package(rm, name, path);
}

ResourcePackageNameHash resource_manager::add_package(
	ResourceManager& rm,
	StringRef const& name,
	StringRef const& path
) {
	auto const name_hash = resource::hash_package_name(name);
	for (auto const* it_pkg : rm._packages) {
		TOGO_ASSERTF(
			name_hash != it_pkg->_name_hash,
			"package '%.*s' already exists",
			name.size, name.data
		);
		TOGO_ASSERTF(
			!string::compare_equal(path, resource_package::path(*it_pkg)),
			"package at '%.*s' already exists",
			path.size, path.data
		);
	}
	Allocator& allocator = *rm._packages._allocator;
	ResourcePackage* const pkg = TOGO_CONSTRUCT(allocator,
		ResourcePackage, name, path, allocator
	);
	array::push_back(rm._packages, pkg);
	resource_package::open(*pkg, rm);
	return resource_package::name_hash(*pkg);
}

void resource_manager::remove_package(
	ResourceManager& rm,
	ResourcePackageNameHash const name_hash
) {
	Allocator& allocator = *rm._packages._allocator;
	for (unsigned i = 0; i < array::size(rm._packages); ++i) {
		auto* const pkg = rm._packages[i];
		if (name_hash == resource_package::name_hash(*pkg)) {
			// TODO: Unload active resources from the package
			resource_package::close(*pkg);
			TOGO_DESTROY(allocator, pkg);
			array::remove(rm._packages, i);
			return;
		}
	}
	TOGO_ASSERT(false, "package not found");
}

void resource_manager::clear_packages(ResourceManager& rm) {
	Allocator& allocator = *rm._packages._allocator;
	// TODO: Unload all active resources
	for (auto* pkg : rm._packages) {
		resource_package::close(*pkg);
		TOGO_DESTROY(allocator, pkg);
	}
	array::clear(rm._packages);
}

void* resource_manager::load_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	TOGO_DEBUG_ASSERTE(hash_map::has(rm._handlers, type));
	auto* tr = hash_map::get(rm._resources, name_hash);
	if (!tr) {
		auto const* const handler = hash_map::get(rm._handlers, type);
		ResourcePackage* pkg = nullptr;
		ResourcePackage::LookupNode* node = nullptr;
		if (!find_resource(rm, type, name_hash, pkg, node)) {
			TOGO_LOG_ERRORF(
				"resource not found: [%08x %016lx]\n",
				type, name_hash
			);
			return nullptr;
		}
		StringRef const pkg_name{resource_package::name(*pkg)};
		IReader* const stream = resource_package::open_resource_stream(
			*pkg, node
		);
		if (!stream) {
			TOGO_LOG_ERRORF(
				"failed to open resource stream from package '%.*s': [%08x %016lx]\n",
				pkg_name.size, pkg_name.data,
				type, name_hash
			);
			return nullptr;
		}
		void* const load_value = handler->handler.func_load(
			handler->type_data, rm, name_hash, *stream
		);
		resource_package::close_resource_stream(*pkg);
		if (!load_value) {
			TOGO_LOG_ERRORF(
				"failed to load resource from package '%.*s': [%08x %016lx]\n",
				pkg_name.size, pkg_name.data,
				type, name_hash
			);
			return nullptr;
		}
		tr = &hash_map::push(
			rm._resources, name_hash, {load_value, type}
		);
	}
	return (tr && tr->type == type) ? tr->value : nullptr;
}

void resource_manager::unload_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	TOGO_DEBUG_ASSERTE(hash_map::has(rm._handlers, type));
	auto* const node = hash_map::get_node(rm._resources, name_hash);
	if (!node || node->value.type != type) {
		return;
	}
	auto const* const handler = hash_map::get(rm._handlers, type);
	handler->handler.func_unload(
		handler->type_data, rm, name_hash, node->value.value
	);
	hash_map::remove(rm._resources, node);
}

void* resource_manager::get_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	TOGO_DEBUG_ASSERTE(hash_map::has(rm._handlers, type));
	auto* const tr = hash_map::get(rm._resources, name_hash);
	return (tr && tr->type == type) ? tr->value : nullptr;
}

} // namespace togo
