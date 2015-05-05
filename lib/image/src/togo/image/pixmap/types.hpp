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

/// Pixel data type.
enum class PixelDataType : unsigned {
	/// Unpacked with 8-bit components.
	c8,
	/// Packed into 32 bits.
	p32,
};

/// Pixel component layout.
enum class PixelLayout : unsigned {
	/// RGB.
	rgb,
	/// RGB + unused byte.
	rgbx,
	/// RGBA.
	rgba,
};

/// Pixel format ID.
enum class PixelFormatID : unsigned {
	/// Unpacked RGB with 8-bit components (24 bits).
	rgb8,
	/// Packed RGB + unused byte with 8-bit components (32 bits).
	rgbx8,
	/// Packed RGBA with 8-bit components (32 bits).
	rgba8,
};

/// Pixel format.
struct PixelFormat {
	enum : unsigned {
		MASK_PROPERTY = 0xFF,
		P_DATA_TYPE = 0x00,
		P_LAYOUT = 0x08,
	};
	unsigned properties;

	/// Check if this format is equivalent to another.
	bool equals(PixelFormat const& other) const {
		return properties == other.properties;
	}

	/// Pixel data type.
	PixelDataType data_type() const {
		return static_cast<PixelDataType>((properties >> P_DATA_TYPE) & MASK_PROPERTY);
	}

	/// Pixel component layout.
	PixelLayout layout() const {
		return static_cast<PixelLayout>((properties >> P_LAYOUT) & MASK_PROPERTY);
	}

	/// Calculate the bytes needed to store a pixmap.
	unsigned bytes_required(UVec2 size) const;
};

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

	/// Construct empty with format by ID.
	Pixmap(PixelFormatID format_id);
	/// Construct empty with format.
	Pixmap(PixelFormat const& format);

	/// Construct with size and format by ID.
	Pixmap(UVec2 size, PixelFormatID format_id);
	/// Construct with size and format.
	Pixmap(UVec2 size, PixelFormat const& format);

	Pixmap(Pixmap&&);
	Pixmap& operator=(Pixmap&&);
};

/** @} */ // end of doc-group lib_image_pixmap

} // namespace togo
