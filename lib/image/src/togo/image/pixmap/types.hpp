#line 2 "togo/image/pixmap/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Pixmap types.
@ingroup lib_image_types
@ingroup lib_image_pixmap
*/

#pragma once

#include <togo/image/config.hpp>
#include <togo/image/types.hpp>
#include <togo/core/math/types.hpp>

namespace togo {

/**
	@addtogroup lib_image_pixmap
	@{
*/

/// Pixel format.
enum class PixelFormatID : unsigned {
	/// RGB, unpacked in 8-bit integrals.
	rgb,

	/// RGBX (unused component), packed in 32-bit integral.
	rgbx,
	/// XRGB (unused component), packed in 32-bit integral.
	xrgb,

	/// RGBA, packed in 32-bit integral.
	rgba,
	/// ARGB, packed in 32-bit integral.
	argb,
};

/// Pixel format.
struct PixelFormat {
	PixelFormatID id;
	// TODO: Endianness? Ideally only use the system endian, but that may
	// be detrimental for RT systems that send pixel data to other systems.
};

/// Pixel format info.
struct PixelFormatInfo {
	/// Whether a component is used, in this order: R, G, B, A.
	u32 comp[4];
	/// The bit index of a component.
	u32 shift[4];
};

/// Pixel format info by ID.
extern PixelFormatInfo const pixel_format_info[];

/// Pixel grid.
struct Pixmap {
	u8* data;
	unsigned data_capacity;
	unsigned data_size;
	UVec2 size;
	PixelFormat format;

	Pixmap() = delete;
	Pixmap(Pixmap const&) = delete;
	Pixmap& operator=(Pixmap const&) = delete;

	~Pixmap();

	/// Construct empty with format.
	Pixmap(PixelFormat format);
	/// Construct empty with format ID.
	Pixmap(PixelFormatID format_id);

	/// Construct with size and format.
	Pixmap(UVec2 size, PixelFormat format);
	/// Construct with size and format ID.
	Pixmap(UVec2 size, PixelFormatID format_id);

	Pixmap(Pixmap&&);
	Pixmap& operator=(Pixmap&&);
};

/** @} */ // end of doc-group lib_image_pixmap

} // namespace togo
