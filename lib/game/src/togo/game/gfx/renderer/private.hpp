#line 2 "togo/game/gfx/renderer/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/game/gfx/renderer/types.hpp>

namespace togo {
namespace game {
namespace gfx {

template<class R>
inline bool operator==(ResourceID<R> const& lhs, ResourceID<R> const& rhs) {
	return lhs._value == rhs._value;
}

template<class R>
inline bool operator!=(ResourceID<R> const& lhs, ResourceID<R> const& rhs) {
	return lhs._value != rhs._value;
}

namespace renderer {

void init_base(gfx::Renderer* renderer);
void teardown_base(gfx::Renderer* renderer);

void configure_base(
	gfx::Renderer* renderer,
	gfx::RenderConfig const& config
);

} // namespace renderer

namespace resource_array {

template<class R, unsigned N>
inline R& get(gfx::ResourceArray<R, N>& ra, gfx::ResourceID<R> const id) {
	using RA = gfx::ResourceArray<R, N>;
	return ra._slots[id._value & RA::INDEX_MASK].res;
}

template<class R, unsigned N>
void clear(gfx::ResourceArray<R, N>& ra) {
	using RA = gfx::ResourceArray<R, N>;
	// NB: Not resetting _id_gen to avoid premature overrun
	ra._num = 0;
	ra._first_hole = ra._slots;
	auto* const last = ra._slots + (RA::NUM_SLOTS - 1);
	for (auto* slot = ra._slots; slot != last; ++slot) {
		slot->id._value = ID_VALUE_NULL;
		slot->hole.next = slot + 1;
	}
	last->hole.next = nullptr;
}

template<class R, unsigned N>
R& assign(gfx::ResourceArray<R, N>& ra, R const& res) {
	using RA = gfx::ResourceArray<R, N>;
	TOGO_ASSERT(ra._num != RA::NUM_SLOTS, "cannot add resource to full array");
	TOGO_DEBUG_ASSERTE(ra._first_hole != nullptr);
	gfx::ResourceSlot<R>& slot = *ra._first_hole;
	ra._first_hole = slot.hole.next;
	slot.res = res;
	slot.res.id._value = ra._id_gen | (&slot - ra._slots);
	ra._id_gen = max(ra._id_gen + RA::ID_ADD, u32{RA::ID_ADD});
	return slot.res;
}

template<class R, unsigned N>
void free(gfx::ResourceArray<R, N>& ra, R& res) {
	using RA = gfx::ResourceArray<R, N>;
	auto& slot = *reinterpret_cast<gfx::ResourceSlot<R>*>(&res);
	unsigned index = slot.id._value & RA::INDEX_MASK;
	slot.hole.next = nullptr;
	while (index--) {
		gfx::ResourceSlot<R>& slot_before = ra._slots[index];
		if (slot_before.id._value == ID_VALUE_NULL) {
			slot.hole.next = slot_before.hole.next;
			slot_before.hole.next = &slot;
			break;
		}
	}
	if (!slot.hole.next) {
		// No hole between slot and head
		slot.hole.next = ra._first_hole;
		ra._first_hole = &slot;
	}
	slot.id._value = ID_VALUE_NULL;
}

} // namespace resource_array

} // namespace gfx
} // namespace game
} // namespace togo
