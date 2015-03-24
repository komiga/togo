#line 2 "togo/gfx/gfx.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Graphics interface.
@ingroup gfx
*/

#pragma once

#include <togo/config.hpp>
#include <togo/string/types.hpp>
#include <togo/hash/hash.hpp>
#include <togo/gfx/types.hpp>
#include <togo/gfx/gfx.gen_interface>

namespace togo {
namespace gfx {

/**
	@addtogroup gfx
	@{
*/

/// Calculate hash of parameter block name.
inline gfx::ParamBlockNameHash hash_param_block_name(StringRef const& name) {
	return hash::calc32(name);
}

/// Calculate hash of generator name.
inline gfx::GeneratorNameHash hash_generator_name(StringRef const& name) {
	return hash::calc32(name);
}

/// Calculate hash of viewport name.
inline gfx::GeneratorNameHash hash_viewport_name(StringRef const& name) {
	return hash::calc32(name);
}

/** @} */ // end of doc-group gfx

} // namespace gfx
} // namespace togo
