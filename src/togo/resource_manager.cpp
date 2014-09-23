#line 2 "togo/resource_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/hash_map.hpp>
#include <togo/resource_manager.hpp>

namespace togo {

ResourceManager::ResourceManager(Allocator& allocator)
	: _loaders(allocator)
	, _resources(allocator)
{
	hash_map::reserve(_loaders, 16);
}

void resource_manager::register_loader(
	ResourceManager& rm,
	ResourceType type,
	void* type_data,
	ResourceLoader::load_func_type& load_func,
	ResourceLoader::unload_func_type& unload_func
) {
	TOGO_ASSERT(
		!hash_map::has(rm._loaders, type),
		"type has already been registered"
	);
	hash_map::set(rm._loaders, type, {
		type_data,
		&load_func,
		&unload_func,
		type
	});
}

void* resource_manager::get_resource(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	void** const value = hash_map::get(rm._resources, name_hash);
	return value ? *value : nullptr;
}

} // namespace togo
