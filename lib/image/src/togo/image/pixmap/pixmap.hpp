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

/// Check if pixmap is empty.
inline bool empty(Pixmap const& p) {
	return p.data_size == 0;
}

/// Fill with color.
inline void fill(Pixmap& p, Color color) {
	pixmap::fill(p, color, UVec4{0, 0, p.size});
}

/// Blit a pixmap (not scaled, not blended).
inline void blit(Pixmap& dst, Pixmap const& src, UVec2 dst_pos) {
	pixmap::blit(dst, src, dst_pos, UVec4{0, 0, src.size});
}

/** @} */ // end of doc-group lib_image_pixmap

} // namespace pixmap
} // namespace togo
