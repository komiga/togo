#line 2 "togo/platform/notification/internal.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Notification internals.
@ingroup lib_platform_types
@ingroup lib_platform_notification
*/

#pragma once

#include <togo/platform/config.hpp>
#include <togo/platform/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/platform/notification/types.hpp>

namespace togo {
namespace notification {

namespace {

enum : unsigned {
	DEFAULT_LIFETIME_FALLBACK = 15000,
};

enum : unsigned {
	S_TYPE = 0,
	B_TYPE = bits_to_store(unsigned_cast(notification::c_num_type) - 1),
	M_TYPE = fill_n_bits(B_TYPE),

	S_PRIORITY = S_TYPE + B_TYPE,
	B_PRIORITY = bits_to_store(unsigned_cast(notification::c_num_priority) - 1),
	M_PRIORITY = fill_n_bits(B_PRIORITY),

	S_MODIFIED = S_PRIORITY + B_PRIORITY,
	B_MODIFIED = 1,
	M_MODIFIED = 1,

	S_POSTED = S_MODIFIED + B_MODIFIED,
	B_POSTED = 1,
	M_POSTED = 1,

	S_LIFETIME = S_POSTED + B_POSTED,
	B_LIFETIME = bits_to_store(unsigned_cast(notification::MAX_LIFETIME)),
	M_LIFETIME = fill_n_bits(B_LIFETIME),

	S_LIFETIME_SIGN = S_LIFETIME + B_LIFETIME,
	B_LIFETIME_SIGN = 1,
	M_LIFETIME_SIGN = 1,
};
static_assert(
	S_LIFETIME_SIGN < (sizeof(Notification::_properties) << 3),
	"properties exceed limit of value type"
);

} // anonymous namespace

namespace internal {

inline unsigned get_property(Notification const& notification, unsigned mask, unsigned shift) {
	return (notification._properties & mask) >> shift;
}

inline void set_property(Notification& notification, unsigned mask, unsigned shift, unsigned value) {
	notification._properties = (notification._properties & ~mask) | (value << shift);
}

} // namespace internal

} // namespace notification
} // namespace togo
