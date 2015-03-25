#line 2 "togo/world/world_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/queue.hpp>
#include <togo/collection/hash_map.hpp>
#include <togo/world/types.hpp>
#include <togo/world/world_manager.hpp>

namespace togo {

namespace {
enum : unsigned {
	MIN_FREE_INDICES = 64,
};
} // anonymous namespace

WorldInstance::WorldInstance(
	Allocator& allocator
)
	: generation(0)
	, component_managers(allocator)
	, entities(allocator)
{}

WorldManager::WorldManager(
	Allocator& allocator
)
	: _component_manager_defs(allocator)
	, _instances(allocator)
	, _free_indices(allocator)
{
	hash_map::reserve(_component_manager_defs, 32);
	array::reserve(_instances, 32);
	queue::reserve(_free_indices, MIN_FREE_INDICES);
}

WorldManager::~WorldManager() {
	world_manager::shutdown(*this);
}

/// Whether a world is alive.
bool world_manager::alive(
	WorldManager const& wm,
	WorldID const& id
) {
	auto const index = id.index();
	return
		index < array::size(wm._instances) &&
		wm._instances[index].generation == id.generation()
	;
}

/// Register a component manager.
///
/// An assertion will fail if the component name has already been
/// registered.
/// Component managers must be registered before any worlds are created.
void world_manager::register_component_manager(
	WorldManager& wm,
	ComponentManagerDef const& def
) {
	TOGO_DEBUG_ASSERTE(
		def.name_hash != hash::IDENTITY32 &&
		def.func_create &&
		def.func_destroy &&
		def.func_clear
	);
	TOGO_ASSERT(
		!hash_map::has(wm._component_manager_defs, def.name_hash),
		"a component manager has already been registered with this name"
	);
	TOGO_ASSERT(
		array::empty(wm._instances),
		"component managers must be registered before any worlds are created"
	);
	hash_map::push(wm._component_manager_defs, def.name_hash, def);
}

/// Create world.
WorldID world_manager::create(WorldManager& wm) {
	WorldID::value_type index;
	if (queue::size(wm._free_indices) > MIN_FREE_INDICES) {
		index = queue::front(wm._free_indices);
		queue::pop_front(wm._free_indices);
	} else {
		index = array::size(wm._instances);
		array::increase_size(wm._instances, 1);
		auto& instance = array::back(wm._instances);
		new(&instance) WorldInstance(*wm._instances._allocator);
		hash_map::reserve(
			instance.component_managers,
			hash_map::size(wm._component_manager_defs)
		);
		for (auto const& entry : wm._component_manager_defs) {
			hash_map::push(instance.component_managers, entry.key, entry.value.func_create(wm));
		}
		TOGO_ASSERTE(index < (1 << WorldID::INDEX_BITS));
	}
	return WorldID{index | (wm._instances[index].generation << WorldID::INDEX_BITS)};
}

/// Destroy world.
void world_manager::destroy(
	WorldManager& wm,
	WorldID const& id
) {
	TOGO_ASSERTE(world_manager::alive(wm, id));
	auto const index = id.index();
	auto& instance = wm._instances[index];
	array::clear(instance.entities);
	for (auto& entry : instance.component_managers) {
		void* data = entry.value;
		auto* def = hash_map::find(wm._component_manager_defs, entry.key);
		TOGO_DEBUG_ASSERTE(def);
		def->func_clear(wm, data);
	}
	++instance.generation;
	queue::push_back(wm._free_indices, index);
}

/// Shutdown.
///
/// Removes all worlds and component manager definitions.
/// This should only be used as part of system deinitialization.
/// Using it during runtime can lead to zombie IDs pointing to valid
/// worlds (i.e., not the world originally created with the ID).
void world_manager::shutdown(WorldManager& wm) {
	for (auto& instance : wm._instances) {
		for (auto const& entry : instance.component_managers) {
			void* data = entry.value;
			auto* def = hash_map::find(wm._component_manager_defs, entry.key);
			TOGO_DEBUG_ASSERTE(def);
			def->func_destroy(wm, data);
		}
		instance.~WorldInstance();
	}
	queue::clear(wm._free_indices);
	array::clear(wm._instances);
	hash_map::clear(wm._component_manager_defs);
}

} // namespace togo
