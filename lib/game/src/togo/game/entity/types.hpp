#line 2 "togo/game/entity/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Entity types.
@ingroup lib_game_types
@ingroup lib_game_entity
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/collection/types.hpp>
#include <togo/core/hash/types.hpp>

namespace togo {

/**
	@addtogroup lib_game_entity
	@{
*/

/// Entity ID.
struct EntityID {
	using value_type = u32;

	enum : unsigned {
		INDEX_BITS = 24,
		INDEX_MASK = (1 << INDEX_BITS) - 1,

		GENERATION_BITS = 8,
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

/// Entity manager.
struct EntityManager {
	unsigned _num_alive;
	Array<u8> _generation;
	Queue<EntityID::value_type> _free_indices;

	EntityManager() = delete;
	EntityManager(EntityManager const&) = delete;
	EntityManager& operator=(EntityManager const&) = delete;

	~EntityManager() = default;
	EntityManager(EntityManager&&) = default;
	EntityManager& operator=(EntityManager&&) = default;

	EntityManager(
		Allocator& allocator
	);
};

/** @} */ // end of doc-group lib_game_entity

} // namespace togo
