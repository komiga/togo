#line 2 "togo/entity/entity_manager.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/collection/array.hpp>
#include <togo/collection/queue.hpp>
#include <togo/entity/types.hpp>
#include <togo/entity/entity_manager.hpp>

namespace togo {

namespace {
enum : unsigned {
	MIN_FREE_INDICES = 1024,
};
} // anonymous namespace

EntityManager::EntityManager(
	Allocator& allocator
)
	: _num_alive(0)
	, _generation(allocator)
	, _free_indices(allocator)
{
	array::reserve(_generation, 4 * 1024);
	queue::reserve(_free_indices, MIN_FREE_INDICES);
}

bool entity_manager::alive(
	EntityManager const& em,
	EntityID const& id
) {
	auto const index = id.index();
	return
		index < array::size(em._generation) &&
		em._generation[index] == id.generation()
	;
}

EntityID entity_manager::create(EntityManager& em) {
	EntityID::value_type index;
	if (queue::size(em._free_indices) > MIN_FREE_INDICES) {
		index = queue::front(em._free_indices);
		queue::pop_front(em._free_indices);
	} else {
		index = array::size(em._generation);
		array::push_back(em._generation, u8{0});
		TOGO_ASSERTE(index < (1 << EntityID::INDEX_BITS));
	}
	++em._num_alive;
	return EntityID{index | (em._generation[index] << EntityID::INDEX_BITS)};
}

void entity_manager::destroy(
	EntityManager& em,
	EntityID const& id
) {
	TOGO_ASSERTE(entity_manager::alive(em, id));
	auto const index = id.index();
	++em._generation[index];
	--em._num_alive;
	if (em._num_alive > 0) {
		queue::push_back(em._free_indices, index);
	} else {
		array::clear(em._generation);
		queue::clear(em._free_indices);
	}
}

void entity_manager::shutdown(EntityManager& em) {
	queue::clear(em._free_indices);
	array::clear(em._generation);
	em._num_alive = 0;
}

} // namespace togo
