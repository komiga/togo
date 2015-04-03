#line 2 "togo/core/utility/tags.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Variation/placeholder tags.
@ingroup utility
*/

#pragma once

#include <togo/core/config.hpp>

namespace togo {

/**
	@addtogroup utility
	@{
*/

/** @name Variation/placeholder tags */ /// @{

/// Null value tag.
enum class null_tag {};

/// Null reference tag.
enum class null_ref_tag {};

/// NUL-terminated string tag.
enum class cstr_tag {};

/// Boolean value tag.
enum class bool_tag {};

/// @}

/** @} */ // end of doc-group utility

} // namespace togo
