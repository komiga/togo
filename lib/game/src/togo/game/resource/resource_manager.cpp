#line 2 "togo/game/resource/resource_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/collection/hash_map.hpp>
#include <togo/game/resource/types.hpp>
#include <togo/game/resource/resource.hpp>
#include <togo/game/resource/resource_package.hpp>
#include <togo/game/resource/resource_manager.hpp>

namespace togo {
namespace game {

namespace resource_manager {

#if defined(TOGO_DEBUG)
#define TOGO_LOG_RESOURCE_MANAGER_ACTION(action, resource_) do {			\
	auto const& metadata = (resource_).metadata;							\
	TOGO_LOG_DEBUGF(														\
		"resource_manager: %6s [%08x %016lx %016lx %-4u] R %-3u\n",			\
		action,																\
		metadata.type,														\
		metadata.name_hash,													\
		metadata.tag_glob_hash,												\
		metadata.id,														\
		resource::num_refs(resource_)										\
	);																		\
} while(false)
#else
#define TOGO_LOG_RESOURCE_MANAGER_ACTION(action, resource_)
#endif

static ResourceManager::ActiveNode* find_active_node(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto* node = hash_map::find_node(rm._resources, name_hash);
	for (; node; node = hash_map::next_node(rm._resources, node)) {
		if (node->value->metadata.type == type) {
			return node;
		}
	}
	return nullptr;
}

static void unload_impl(
	ResourceManager& rm,
	Resource& resource,
	ResourceHandler const* handler
) {
	TOGO_LOG_RESOURCE_MANAGER_ACTION("unload", resource);
	handler->func_unload(handler->type_data, rm, resource);
	resource.value = nullptr;
	resource.properties &= ~Resource::F_ACTIVE;
}

static unsigned unload_package_impl(
	ResourceManager& rm,
	ResourcePackage& pkg
) {
	// TODO: optimize?
	if (hash_map::empty(rm._resources)) {
		return 0;
	}
	unsigned num = 0;
	auto* const head = begin(pkg._manifest);
	auto* const tail = end(pkg._manifest);

	auto& resources = rm._resources._data;
	ResourceType type = RES_TYPE_NULL;
	ResourceHandler const* handler;
	for (unsigned i = 0; i < array::size(resources); ++i) {
		auto& node = resources[i];
		auto* resource = node.value;
		if (resource >= head && resource < tail) {
			if (resource->metadata.type != type) {
				type = resource->metadata.type;
				handler = hash_map::find(rm._handlers, type);
				TOGO_DEBUG_ASSERTE(handler);
			}
			resource_manager::unload_impl(rm, *resource, handler);
			hash_map::remove(rm._resources, &node);
			++num;
		}
	}
	return num;
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
	TOGO_DEBUG_ASSERTE(base_path.any());
	hash_map::reserve(_handlers, 16);
	string::copy(_base_path, base_path);
	string::ensure_trailing_slash(_base_path);
}

/// Register resource handler.
///
/// An assertion will fail if a handler for the type has already
/// been registered.
void resource_manager::register_handler(
	ResourceManager& rm,
	ResourceHandler const& handler
) {
	TOGO_ASSERT(
		handler.func_load && handler.func_unload,
		"func_load and func_unload must be assigned in handler"
	);
	TOGO_ASSERT(
		!hash_map::has(rm._handlers, handler.type),
		"type has already been registered"
	);
	hash_map::push(rm._handlers, handler.type, handler);
}

/// Remove all resource handlers.
void resource_manager::clear_handlers(ResourceManager& rm) {
	TOGO_DEBUG_ASSERTE(hash_map::empty(rm._resources));
	hash_map::clear(rm._handlers);
}

/// Whether a resource handler is registered.
bool resource_manager::has_handler(
	ResourceManager const& rm,
	ResourceType const type
) {
	return hash_map::has(rm._handlers, type);
}

/// Add package by name and base path.
///
/// Returns package name hash.
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

/// Add package by name and path.
///
/// Returns package name hash.
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

/// Unload all active resources in package.
///
/// Returns the number of resources unloaded.
unsigned resource_manager::unload_package(
	ResourceManager& rm,
	ResourcePackageNameHash const name_hash
) {
	for (unsigned i = 0; i < array::size(rm._packages); ++i) {
		auto* const pkg = rm._packages[i];
		if (name_hash == resource_package::name_hash(*pkg)) {
			return resource_manager::unload_package_impl(rm, *pkg);
		}
	}
	TOGO_ASSERT(false, "package not found");
}

/// Remove package.
void resource_manager::remove_package(
	ResourceManager& rm,
	ResourcePackageNameHash const name_hash
) {
	Allocator& allocator = *rm._packages._allocator;
	for (unsigned i = 0; i < array::size(rm._packages); ++i) {
		auto* const pkg = rm._packages[i];
		if (name_hash == resource_package::name_hash(*pkg)) {
			resource_manager::unload_package_impl(rm, *pkg);
			resource_package::close(*pkg);
			TOGO_DESTROY(allocator, pkg);
			array::remove(rm._packages, i);
			return;
		}
	}
	TOGO_ASSERT(false, "package not found");
}

/// Remove all packages.
void resource_manager::clear_packages(ResourceManager& rm) {
	Allocator& allocator = *rm._packages._allocator;
	resource_manager::clear_resources(rm);
	for (auto* pkg : rm._packages) {
		resource_package::close(*pkg);
		TOGO_DESTROY(allocator, pkg);
	}
	array::clear(rm._packages);
}

/// Whether a resource matching (type, name_hash) exists.
bool resource_manager::has(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto const* resource = resource_manager::find_manifest(
		rm, type, name_hash, nullptr
	);
	return resource != nullptr;
}

/// Load resource.
///
/// The resource is not reloaded if it is already loaded.
Resource* resource_manager::load(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	TOGO_DEBUG_ASSERTE(hash_map::has(rm._handlers, type));
	{// Lookup existing value
	auto* active = resource_manager::find_active(rm, type, name_hash);
	if (active) {
		return active;
	}}

	auto const* const handler = hash_map::find(rm._handlers, type);
	ResourcePackage* pkg = nullptr;
	auto* resource = resource_manager::find_manifest(
		rm, type, name_hash, &pkg
	);
	if (!resource) {
		TOGO_LOG_ERRORF(
			"resource not found: [%08x %016lx]\n",
			type, name_hash
		);
		return nullptr;
	}

	auto& metadata = resource->metadata;
	if (handler->format_version != metadata.data_format_version) {
		TOGO_LOG_ERRORF(
			"resource handler format version mismatch against [%08x %016lx]: %u != %u\n",
			type, name_hash, handler->format_version, metadata.data_format_version
		);
		return nullptr;
	}
	ResourceValue const load_value = handler->func_load(
		handler->type_data, rm, *pkg, *resource
	);
	StringRef const pkg_name{resource_package::name(*pkg)};
	if (!load_value.valid()) {
		TOGO_LOG_ERRORF(
			"failed to load resource from package '%.*s': [%08x %016lx]\n",
			pkg_name.size, pkg_name.data,
			type, name_hash
		);
		return nullptr;
	}
	TOGO_LOG_RESOURCE_MANAGER_ACTION("load", *resource);
	resource->value = load_value;
	resource->properties |= Resource::F_ACTIVE;
	hash_map::push(rm._resources, name_hash, resource);
	return resource;
}

/// Load resource and add reference.
///
/// The resource is not reloaded if it is already loaded.
Resource* resource_manager::ref_load(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto* resource = resource_manager::load(rm, type, name_hash);
	if (resource) {
		resource_manager::ref(rm, *resource);
	}
	return resource;
}

/// Unload resource.
void resource_manager::unload(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	TOGO_DEBUG_ASSERTE(hash_map::has(rm._handlers, type));
	auto* const node = resource_manager::find_active_node(rm, type, name_hash);
	if (node) {
		auto& resource = *node->value;
		auto const* const handler = hash_map::find(rm._handlers, type);
		resource_manager::unload_impl(rm, resource, handler);
		hash_map::remove(rm._resources, node);
	}
}

/// Unload all resources.
void resource_manager::clear_resources(ResourceManager& rm) {
	ResourceType type = RES_TYPE_NULL;
	ResourceHandler const* handler;
	for (auto const& node : rm._resources) {
		auto& resource = *node.value;
		if (resource.metadata.type != type) {
			type = resource.metadata.type;
			handler = hash_map::find(rm._handlers, type);
			TOGO_DEBUG_ASSERTE(handler);
		}
		// FIXME: func_unload() may make invalid requests as we aren't
		// removing the nodes in-loop. Block find_active_node() requests
		// during clear or remove nodes?
		resource_manager::unload_impl(rm, resource, handler);
	}
	hash_map::clear(rm._resources);
}

/// Find resource by type and name.
Resource* resource_manager::find_manifest(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash,
	ResourcePackage** package
) {
	if (array::empty(rm._packages)) {
		return nullptr;
	}
	ResourcePackage::LookupNode* node;
	for (
		auto* it_pkg = array::end(rm._packages) - 1;
		it_pkg >= array::begin(rm._packages);
		--it_pkg
	) {
		node = resource_package::find_node(**it_pkg, name_hash);
		if (!node) {
			continue;
		}
		auto& resource = resource_package::resource(
			**it_pkg, node->value
		);
		// TODO: Tag filter
		if (resource.metadata.type == type) {
			if (package) {
				*package = *it_pkg;
			}
			return const_cast<Resource*>(&resource);
		}
	}
	return nullptr;
}

/// Find active resource by type and name.
Resource* resource_manager::find_active(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto const* const node = resource_manager::find_active_node(rm, type, name_hash);
	return node ? node->value : nullptr;
}

/// Add a resource reference.
///
/// Returns resource's number of references.
unsigned resource_manager::ref(
	ResourceManager& /*rm*/,
	Resource& resource
) {
	unsigned num_refs = resource::num_refs(resource);
	resource::set_num_refs(resource, ++num_refs);
	TOGO_LOG_RESOURCE_MANAGER_ACTION("ref", resource);
	return num_refs;
}

/// Remove a resource reference.
///
/// Returns resource's number of references.
unsigned resource_manager::unref(
	ResourceManager& rm,
	Resource& resource
) {
	// TODO: queue for task?
	unsigned num_refs = resource::num_refs(resource);
	TOGO_DEBUG_ASSERTE(num_refs > 0);
	resource::set_num_refs(resource, --num_refs);
	TOGO_LOG_RESOURCE_MANAGER_ACTION("unref", resource);

	if (num_refs == 0) {
		resource_manager::unload(rm, resource.metadata.type, resource.metadata.name_hash);
	}
	return num_refs;
}

/// Remove a resource reference.
///
/// Returns resource's number of references.
unsigned resource_manager::unref(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash,
	bool const strict IGEN_DEFAULT(true)
) {
	auto* resource = resource_manager::find_active(rm, type, name_hash);
	if (resource) {
		return resource_manager::unref(rm, *resource);
	} else {
		TOGO_ASSERT(strict, "resource not found (strict unref)");
	}
	return 0;
}

} // namespace game
} // namespace togo
