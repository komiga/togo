#line 2 "togo/resource_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/memory.hpp>
#include <togo/array.hpp>
#include <togo/hash_map.hpp>
#include <togo/log.hpp>
#include <togo/resource_types.hpp>
#include <togo/resource_package.hpp>
#include <togo/resource_manager.hpp>

namespace togo {

namespace resource_manager {

static bool find_resource(
	ResourceManager& rm,
	ResourceType const type,
	ResourceNameHash const name_hash,
	ResourcePackage*& package,
	ResourcePackage::EntryNode*& node
) {
	// TODO: Tag filter
	ResourcePackage::EntryNode* it_node;
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

ResourceManager::ResourceManager(Allocator& allocator)
	: _handlers(allocator)
	, _resources(allocator)
	, _packages(allocator)
{
	hash_map::reserve(_handlers, 16);
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

hash64 resource_manager::add_package(
	ResourceManager& rm,
	StringRef const& root
) {
	// NB: ResourcePackage ctor does modify root
	Allocator& allocator = *rm._packages._allocator;
	ResourcePackage* const pkg = TOGO_CONSTRUCT(allocator,
		ResourcePackage, root, allocator
	);
	for (auto const* it_pkg : rm._packages) {
		TOGO_ASSERTF(
			it_pkg->_root_hash != pkg->_root_hash,
			"package at '%.*s' has already been added",
			root.size, root.data
		);
	}
	array::push_back(rm._packages, pkg);
	resource_package::load_manifest(*pkg, rm);
	return resource_package::root_hash(*pkg);
}

void resource_manager::remove_package(
	ResourceManager& rm,
	hash64 const root_hash
) {
	Allocator& allocator = *rm._packages._allocator;
	for (unsigned i = 0; i < array::size(rm._packages); ++i) {
		auto* const pkg = rm._packages[i];
		if (root_hash == pkg->_root_hash) {
			// TODO: Unload active resources from the package
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
		ResourcePackage::EntryNode* node = nullptr;
		if (!find_resource(rm, type, name_hash, pkg, node)) {
			TOGO_LOG_ERRORF(
				"resource not found: %08x %016lx\n",
				type, name_hash
			);
			return nullptr;
		}
		StringRef const pkg_root_ref{resource_package::root(*pkg)};
		StringRef const rpath_ref{node->value.path, node->value.path_size};
		IReader* stream = resource_package::open_resource_stream(
			*pkg, node
		);
		if (!stream) {
			TOGO_LOG_ERRORF(
				"failed to open resource stream from package '%.*s': '%.*s'\n",
				pkg_root_ref.size, pkg_root_ref.data,
				rpath_ref.size, rpath_ref.data
			);
			return nullptr;
		}
		void* const load_value = handler->handler.func_load(
			handler->type_data, rm, name_hash, *stream
		);
		resource_package::close_resource_stream(*pkg);
		if (!load_value) {
			TOGO_LOG_ERRORF(
				"failed to load resource from package '%.*s': '%.*s'\n",
				pkg_root_ref.size, pkg_root_ref.data,
				rpath_ref.size, rpath_ref.data
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
