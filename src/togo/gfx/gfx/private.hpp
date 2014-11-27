#line 2 "togo/gfx/gfx/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>

namespace togo {
namespace gfx {

void init_impl(
	unsigned context_major,
	unsigned context_minor
);

void shutdown_impl();

} // namespace gfx
} // namespace togo
