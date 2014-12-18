#line 2 "togo/resource/resource_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource.hpp>
#include <togo/resource/resource_package.hpp>
#include <togo/resource/resource_manager.hpp>

namespace togo {

namespace resource_manager {

static ResourceMetadata const* resource_metadata(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash,
	ResourcePackage*& package
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
		node = resource_package::get_node(**it_pkg, name_hash);
		if (!node) {
			continue;
		}
		ResourceMetadata const& metadata = resource_package::resource_metadata(
			**it_pkg, node->value
		);
		// TODO: Tag filter
		if (metadata.type == type) {
			package = *it_pkg;
			return &metadata;
		}
	}
	return nullptr;
}

static ResourceManager::ActiveNode* get_active_node(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto* node = hash_map::get_node(rm._resources, name_hash);
	for (; node; node = hash_map::get_next(rm._resources, node)) {
		if (node->value.type == type) {
			return node;
		}
	}
	return nullptr;
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

void resource_manager::clear_handlers(ResourceManager& rm) {
	TOGO_DEBUG_ASSERTE(hash_map::empty(rm._resources));
	hash_map::clear(rm._handlers);
}

bool resource_manager::has_handler(
	ResourceManager const& rm,
	ResourceType const type
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
	resource_manager::clear_resources(rm);
	for (auto* pkg : rm._packages) {
		resource_package::close(*pkg);
		TOGO_DESTROY(allocator, pkg);
	}
	array::clear(rm._packages);
}

bool resource_manager::has_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	ResourcePackage* pkg = nullptr;
	ResourceMetadata const* metadata = resource_manager::resource_metadata(
		rm, type, name_hash, pkg
	);
	return metadata != nullptr;
}

ResourceValue resource_manager::load_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	TOGO_DEBUG_ASSERTE(hash_map::has(rm._handlers, type));
	{// Lookup existing value
	auto const existing_value = resource_manager::get_resource(rm, type, name_hash);
	if (existing_value.valid()) {
		return existing_value;
	}}

	// Load
	auto const* const handler = hash_map::get(rm._handlers, type);
	ResourcePackage* pkg = nullptr;
	ResourceMetadata const* metadata = resource_manager::resource_metadata(
		rm, type, name_hash, pkg
	);
	if (!metadata) {
		TOGO_LOG_ERRORF(
			"resource not found: [%08x %016lx]\n",
			type, name_hash
		);
		return nullptr;
	}
	if (handler->format_version != metadata->data_format_version) {
		TOGO_LOG_ERRORF(
			"resource handler format version mismatch against [%08x %016lx]: %u != %u\n",
			type, name_hash, handler->format_version, metadata->data_format_version
		);
	}
	StringRef const pkg_name{resource_package::name(*pkg)};
	ResourceValue const load_value = handler->func_load(
		handler->type_data, rm, *pkg, *metadata
	);
	if (!load_value.valid()) {
		TOGO_LOG_ERRORF(
			"failed to load resource from package '%.*s': [%08x %016lx]\n",
			pkg_name.size, pkg_name.data,
			type, name_hash
		);
		return nullptr;
	}
	hash_map::push(rm._resources, name_hash, {load_value, type});
	return load_value;
}

void resource_manager::unload_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	TOGO_DEBUG_ASSERTE(hash_map::has(rm._handlers, type));
	auto* const node = resource_manager::get_active_node(rm, type, name_hash);
	if (node) {
		auto const* const handler = hash_map::get(rm._handlers, type);
		handler->func_unload(handler->type_data, rm, node->value.value);
		hash_map::remove(rm._resources, node);
	}
}

void resource_manager::clear_resources(ResourceManager& rm) {
	ResourceType type = RES_TYPE_NULL;
	ResourceHandler const* handler;
	for (auto const& node : rm._resources) {
		if (node.value.type != type) {
			type = node.value.type;
			handler = hash_map::get(rm._handlers, type);
			TOGO_DEBUG_ASSERTE(handler);
		}
		// FIXME: func_unload() may make invalid requests as we aren't
		// removing the nodes in-loop. Block get_active_node() requests
		// during clear or remove nodes?
		handler->func_unload(handler->type_data, rm, node.value.value);
	}
	hash_map::clear(rm._resources);
}

ResourceValue resource_manager::get_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash
) {
	auto const* const node = resource_manager::get_active_node(rm, type, name_hash);
	return node ? node->value.value : nullptr;
}

} // namespace togo
