#line 2 "togo/game/world/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief World types.
@ingroup lib_game_types
@ingroup lib_game_world
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/hash/types.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/game/entity/types.hpp>

namespace togo {

// Forward declarations
namespace game {
	struct WorldManager;
	struct WorldInstance;
} // namespace game

template<>
struct allow_collection_value_type<game::WorldInstance> : true_type {};

namespace game {

/**
	@addtogroup lib_game_world
	@{
*/

/// Component name hasher.
using ComponentNameHasher = hash::Default32;

/// Component name hash.
using ComponentNameHash = ComponentNameHasher::Value;

/// Component name hash literal.
inline constexpr ComponentNameHash
operator"" _component_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc_ce<ComponentNameHasher>(data, size);
}

/// Component manager definition.
struct ComponentManagerDef {
	using create_func_type = void* (
		WorldManager& world_manager
	);
	using destroy_func_type = void (
		WorldManager& world_manager,
		void* data
	);
	using clear_func_type = void (
		WorldManager& world_manager,
		void* data
	);

	ComponentNameHash name_hash;

	/// Create component manager for a world.
	create_func_type* func_create;
	/// Destroy component manager.
	destroy_func_type* func_destroy;
	/// Clear component manager.
	clear_func_type* func_clear;
};

/// World ID.
struct WorldID {
	using value_type = u32;

	enum : unsigned {
		INDEX_BITS = 16,
		INDEX_MASK = (1 << INDEX_BITS) - 1,

		GENERATION_BITS = 16,
		GENERATION_MASK = (1 << GENERATION_BITS) - 1,
	};

	// layout: [generation] [index]
	value_type value;

	/// Index.
	unsigned index() const {
		return value & INDEX_MASK;
	}

	/// Generation.
	unsigned generation() const {
		return (value >> INDEX_BITS) & GENERATION_MASK;
	}
};

struct WorldInstance {
	u16 generation;
	HashMap<ComponentNameHash, void*> component_managers;
	Array<EntityID> entities;

	WorldInstance() = delete;
	WorldInstance(WorldInstance const&) = delete;
	WorldInstance& operator=(WorldInstance const&) = delete;

	~WorldInstance() = default;
	WorldInstance(WorldInstance&&) = default;
	WorldInstance& operator=(WorldInstance&&) = default;

	WorldInstance(
		Allocator& allocator
	);
};

/// World manager.
struct WorldManager {
	HashMap<ComponentNameHash, ComponentManagerDef> _component_manager_defs;
	Array<WorldInstance> _instances;
	Queue<WorldID::value_type> _free_indices;

	WorldManager() = delete;
	WorldManager(WorldManager const&) = delete;
	WorldManager& operator=(WorldManager const&) = delete;

	WorldManager(WorldManager&&) = default;
	WorldManager& operator=(WorldManager&&) = default;

	~WorldManager();
	WorldManager(
		Allocator& allocator
	);
};

/** @} */ // end of doc-group lib_game_world

} // namespace game

} // namespace togo
