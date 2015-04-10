#line 2 "togo/game/gfx/gfx/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/game/config.hpp>

namespace togo {
namespace game {
namespace gfx {

void init_impl(
	unsigned context_major,
	unsigned context_minor
);

void shutdown_impl();

} // namespace gfx
} // namespace game
} // namespace togo
