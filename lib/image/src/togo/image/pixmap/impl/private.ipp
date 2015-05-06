#line 2 "togo/image/pixmap/impl/private.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/image/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/image/pixmap/pixmap.hpp>
#include <togo/image/pixmap/impl/private.hpp>

#include <cstring>

namespace togo {

namespace pixmap {

unsigned const num_components[]{
	3, // rgb
	4, // rgbx
	4, // rgba
};

#define FMT_PROPERTIES(data_type, layout) ( \
	(unsigned_cast(data_type) << PixelFormat::P_DATA_TYPE) | \
	(unsigned_cast(layout) << PixelFormat::P_LAYOUT) \
)

PixelFormat const pixel_formats[]{
	{FMT_PROPERTIES(PixelDataType::c8 , PixelLayout::rgb )}, // rgb8
	{FMT_PROPERTIES(PixelDataType::p32, PixelLayout::rgbx)}, // rgbx8
	{FMT_PROPERTIES(PixelDataType::p32, PixelLayout::rgba)}, // rgba8
};

#undef FMT_PROPERTIES

} // namespace pixmap

// private

void pixmap::fill_c8(Pixmap& p, Color& color, UVec4& rect) {
	unsigned pixel_size = 1 * num_components[unsigned_cast(p.format.layout())];
	unsigned num_rows = rect.height;
	unsigned bytes_to_next_row = (p.size.width - rect.width) * pixel_size;
	u8* dst = p.data + (rect.y * p.size.width + rect.x) * pixel_size;
	unsigned n;
	switch (p.format.layout()) {
	case PixelLayout::rgb:
		while (num_rows--) {
			n = rect.width;
			while (n--) {
				*dst++ = color.r;
				*dst++ = color.g;
				*dst++ = color.b;
			}
			dst += bytes_to_next_row;
		}
		break;

	case PixelLayout::rgbx:
		while (num_rows--) {
			n = rect.width;
			while (n--) {
				*dst++ = color.r;
				*dst++ = color.g;
				*dst++ = color.b;
				dst++;
			}
			dst += bytes_to_next_row;
		}
		break;

	case PixelLayout::rgba:
		while (num_rows--) {
			n = rect.width;
			while (n--) {
				*dst++ = color.r;
				*dst++ = color.g;
				*dst++ = color.b;
				*dst++ = color.a;
			}
			dst += bytes_to_next_row;
		}
		break;
	}
}

void pixmap::fill_p32(Pixmap& p, Color& color, UVec4& rect) {
	enum : unsigned { pixel_size = 4 };
	unsigned num_rows = rect.height;
	unsigned bytes_to_next_row = (p.size.width - rect.width) * pixel_size;
	u32* dst = pointer_add(
		reinterpret_cast<u32*>(p.data),
		(rect.y * p.size.width + rect.x) * pixel_size
	);
	u32 packed = color.packed();
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

void pixmap::blit_unscaled_c8(
	PixelFormat const& format,
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
) {
	unsigned pixel_size = 1 * num_components[unsigned_cast(format.layout())];
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

void pixmap::blit_unscaled_p32(
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
) {
	enum : unsigned { pixel_size = 4 };
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
	if (dst_format.equals(src_format)) {
		switch (dst_format.data_type()) {
		case PixelDataType::c8:
			pixmap::blit_unscaled_c8(
				dst_format,
				dst, dst_width, dst_pos,
				src, src_width, src_rect
			);
			return;

		case PixelDataType::p32:
			pixmap::blit_unscaled_p32(
				dst, dst_width, dst_pos,
				src, src_width, src_rect
			);
			return;
		}
	} else {
		TOGO_ASSERT(false, "TODO");
	}
}

} // namespace togo
