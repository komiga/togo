#line 2 "togo/world/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief World types.
@ingroup types
@ingroup world
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/utility/traits.hpp>
#include <togo/memory/types.hpp>
#include <togo/collection/types.hpp>
#include <togo/hash/types.hpp>
#include <togo/hash/hash.hpp>
#include <togo/entity/types.hpp>

namespace togo {

/**
	@addtogroup world
	@{
*/

struct WorldID;
struct WorldInstance;
struct WorldManager;

/// Component name hash.
using ComponentNameHash = hash32;

/// Component name hash literal.
inline constexpr ComponentNameHash
operator"" _component_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
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

template<>
struct allow_collection_value_type<WorldInstance> : true_type {};

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

/** @} */ // end of doc-group world

} // namespace togo
