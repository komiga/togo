#line 2 "togo/image/pixmap/impl/private.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/image/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/image/pixmap/pixmap.hpp>
#include <togo/image/pixmap/impl/private.hpp>

#include <cstring>

namespace togo {

// private

void pixmap::fill_rgb(Pixmap& p, Color& color, UVec4& rect) {
	enum : unsigned { pixel_size = 3 };
	unsigned num_rows = rect.height;
	unsigned bytes_to_next_row = (p.size.width - rect.width) * pixel_size;
	u8* dst = p.data + (rect.y * p.size.width + rect.x) * pixel_size;
	unsigned n;
	while (num_rows--) {
		n = rect.width;
		while (n--) {
			*dst++ = color.r;
			*dst++ = color.g;
			*dst++ = color.b;
		}
		dst += bytes_to_next_row;
	}
}

void pixmap::fill_packed(Pixmap& p, Color& color, UVec4& rect) {
	enum : unsigned { pixel_size = 4 };
	unsigned num_rows = rect.height;
	unsigned bytes_to_next_row = (p.size.width - rect.width) * pixel_size;
	u32* dst = pointer_add(
		reinterpret_cast<u32*>(p.data),
		(rect.y * p.size.width + rect.x) * pixel_size
	);
	u32 packed = pixel_format::pack(p.format, color);
	unsigned n;
	while (num_rows--) {
		n = rect.width;
		while (n >= 4) {
			*dst++ = packed;
			*dst++ = packed;
			*dst++ = packed;
			*dst++ = packed;
			n -= 4;
		}
		switch (n) {
		case 3: *dst++ = packed;
		case 2: *dst++ = packed;
		case 1: *dst++ = packed;
		case 0: dst += bytes_to_next_row;
		}
	}
}

void pixmap::blit_unscaled_copy(
	PixelFormat const& format,
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
) {
	unsigned pixel_size = pixel_format::pixel_size(format);
	unsigned num_rows = src_rect.height;
	unsigned num_bytes = src_rect.width * pixel_size;

	dst += (dst_pos.y * dst_width + dst_pos.x) * pixel_size;
	src += (src_rect.y * src_width + src_rect.x) * pixel_size;
	dst_width *= pixel_size;
	src_width *= pixel_size;
	// TODO: Optimize for small areas
	while (num_rows--) {
		std::memmove(dst, src, num_bytes);
		dst += dst_width;
		src += src_width;
	}
}

void pixmap::blit_unscaled_choose(
	PixelFormat const& dst_format,
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	PixelFormat const& src_format,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
) {
	if (pixel_format::compare_equal(dst_format, src_format)) {
		pixmap::blit_unscaled_copy(
			dst_format,
			dst, dst_width, dst_pos,
			src, src_width, src_rect
		);
	} else {
		TOGO_ASSERT(false, "TODO: unequal pixmap formats");
	}
}

} // namespace togo
