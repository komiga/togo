#line 2 "togo/image/pixmap/pixmap.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Pixmap interface.
@ingroup lib_image_types
@ingroup lib_image_pixmap
*/

#pragma once

#include <togo/image/config.hpp>
#include <togo/core/math/types.hpp>
#include <togo/image/pixmap/types.hpp>
#include <togo/image/pixmap/pixmap.gen_interface>

namespace togo {
namespace pixmap {

/**
	@addtogroup lib_image_pixmap
	@{
*/

/// Fill with color.
inline void fill(Pixmap& p, Color color) {
	pixmap::fill(p, color, UVec4{0, 0, p.size});
}

/** @} */ // end of doc-group lib_image_pixmap

} // namespace pixmap
} // namespace togo
