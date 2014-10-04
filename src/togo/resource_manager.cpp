#line 2 "togo/resource_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/hash_map.hpp>
#include <togo/resource_types.hpp>
#include <togo/resource_manager.hpp>

namespace togo {

ResourceManager::ResourceManager(Allocator& allocator)
	: _handlers(allocator)
	, _resources(allocator)
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
		"load_func and unload_func must be assigned in handler"
	);
	TOGO_ASSERT(
		!hash_map::has(rm._handlers, handler.type),
		"type has already been registered"
	);
	hash_map::push(rm._handlers, handler.type, {handler, type_data});
}

void* resource_manager::get_resource(
	ResourceManager& rm,
	ResourceNameHash const name_hash
) {
	void** const value = hash_map::get(rm._resources, name_hash);
	return value ? *value : nullptr;
}

} // namespace togo
