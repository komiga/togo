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
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/types.hpp>
#include <togo/image/pixmap/types.hpp>
#include <togo/image/pixmap/pixmap.gen_interface>

namespace togo {

namespace pixel_format {

/**
	@addtogroup lib_image_pixmap
	@{
*/

/// Check if formats are equivalent.
inline bool compare_equal(PixelFormat const& x, PixelFormat const& y) {
	return x.id == y.id;
}

/// The size in bytes of a pixel of this format.
inline unsigned pixel_size(PixelFormat const& format) {
	return format.id == PixelFormatID::rgb ? 3 : 4;
}

/// Calculate the bytes needed to store a pixmap.
inline unsigned bytes_required(PixelFormat const& format, UVec2 size) {
	return size.height * size.width * pixel_format::pixel_size(format);
}

/// Pack color into pixel.
inline u32 pack(PixelFormat const& format, Color const& color) {
	auto& info = pixel_format_info[unsigned_cast(format.id)];
	return
		(color.r << info.shift[0]) |
		(color.g << info.shift[1]) |
		(color.b << info.shift[2]) |
		info.comp[3] ? (color.a << info.shift[3]) : 0
	;
}

/// Unpack pixel into color.
inline Color unpack(PixelFormat const& format, u32 p) {
	auto& info = pixel_format_info[unsigned_cast(format.id)];
	return {
		static_cast<u8>((p >> info.shift[0]) & 0xFF),
		static_cast<u8>((p >> info.shift[1]) & 0xFF),
		static_cast<u8>((p >> info.shift[2]) & 0xFF),
		static_cast<u8>(info.comp[3] ? ((p >> info.shift[3]) & 0xFF) : 0xFF)
	};
}

/** @} */ // end of doc-group lib_image_pixmap

} // namespace pixel_format

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
