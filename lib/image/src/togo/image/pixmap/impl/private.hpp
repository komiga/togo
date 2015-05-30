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

void fill_rgb(Pixmap& p, Color& color, UVec4& rect);
void fill_packed(Pixmap& p, Color& color, UVec4& rect);

void blit_unscaled_copy(
	PixelFormat const& format,
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
);

void blit_unscaled_choose(
	PixelFormat const& dst_format,
	u8* dst,
	unsigned dst_width,
	UVec2 dst_pos,
	PixelFormat const& src_format,
	u8 const* src,
	unsigned src_width,
	UVec4 src_rect
);

} // namespace pixmap
} // namespace togo
