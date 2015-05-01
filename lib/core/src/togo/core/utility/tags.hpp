#line 2 "togo/core/utility/tags.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Variation/placeholder tags.
@ingroup lib_core_utility
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup lib_core_utility
	@{
*/

/** @name Variation/placeholder tags */ /// @{

/// No-initialize constructor tag.
enum class no_init_tag {};

/// Null value tag.
enum class null_tag {};

/// Null reference tag.
enum class null_ref_tag {};

/// NUL-terminated string tag.
enum class cstr_tag {};

/// Boolean value tag.
enum class bool_tag {};

/// @}

/** @} */ // end of doc-group lib_core_utility

} // namespace togo
