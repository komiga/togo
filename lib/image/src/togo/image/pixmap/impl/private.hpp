#line 2 "togo/image/pixmap/impl/private.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/image/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/image/pixmap/types.hpp>

namespace togo {
namespace pixmap {

extern unsigned const num_components[];
extern PixelFormat const pixel_formats[];

void fill_c8(Pixmap& p, Color& color, UVec4& rect);
void fill_p32(Pixmap& p, Color& color, UVec4& rect);

void blit_direct_unscaled_c8(
	PixelFormat const& format,
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
);

void blit_direct_unscaled_p32(
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
);

inline void blit_direct_unscaled(
	PixelFormat const& format,
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
) {
	switch (format.data_type()) {
	case PixelDataType::c8:
		pixmap::blit_direct_unscaled_c8(
			format,
			dst, dst_width, dst_pos,
			src, src_width, src_rect
		);
		return;

	case PixelDataType::p32:
		pixmap::blit_direct_unscaled_p32(
			dst, dst_width, dst_pos,
			src, src_width, src_rect
		);
		return;
	}
	TOGO_DEBUG_ASSERT(false, "invalid format data type");
}

} // namespace pixmap
} // namespace togo
