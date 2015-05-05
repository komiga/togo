#line 2 "togo/image/pixmap/pixmap.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/image/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/math.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/image/pixmap/pixmap.hpp>
#include <togo/image/pixmap/impl/private.ipp>

#include <cstring>

namespace togo {

// PixelFormat implementation

unsigned PixelFormat::bytes_required(UVec2 size) const {
	TOGO_DEBUG_ASSERTE(size.width > 0 && size.height > 0);
	unsigned num_pixels = size.height * size.width;
	switch (this->data_type()) {
	case PixelDataType::c8:
		return num_pixels * 1 * pixmap::num_components[unsigned_cast(this->layout())];
	case PixelDataType::p32:
		return num_pixels * 4;
	}
	TOGO_ASSERTE(false);
}

// Pixmap interface

Pixmap::~Pixmap() {
	pixmap::free(*this);
}

Pixmap::Pixmap(PixelFormatID format_id)
	: data(nullptr)
	, data_capacity(0)
	, data_size(0)
	, size(0, 0)
	, format(pixmap::pixel_formats[unsigned_cast(format_id)])
{}

Pixmap::Pixmap(PixelFormat const& format)
	: data(nullptr)
	, data_capacity(0)
	, data_size(0)
	, size(0, 0)
	, format(format)
{}

Pixmap::Pixmap(UVec2 size, PixelFormatID format_id)
	: data(nullptr)
	, data_capacity(0)
	, data_size(0)
	, size(0, 0)
	, format(pixmap::pixel_formats[unsigned_cast(format_id)])
{
	pixmap::resize(*this, size);
}

Pixmap::Pixmap(UVec2 size, PixelFormat const& format)
	: data(nullptr)
	, data_capacity(0)
	, data_size(0)
	, size(0, 0)
	, format(format)
{
	pixmap::resize(*this, size);
}

Pixmap::Pixmap(Pixmap&&) {
	pixmap::free(*this);
}

Pixmap& Pixmap::operator=(Pixmap&& other) {
	pixmap::free(*this);
	this->data = other.data;
	this->format = other.format;
	this->size = other.size;
	other.data = nullptr;
	other.size = UVec2{0, 0};
	return *this;
}

/// Resize.
///
/// If resized to (0, 0), the data buffer is retained.
/// Existing pixels are retained, but not stretched.
/// New area is filled with fill_color.
void pixmap::resize(
	Pixmap& p,
	UVec2 size,
	Color const fill_color IGEN_DEFAULT(Color(0,0,0,0))
) {
	UVec2 const old_size = p.size;
	if (size.width == 0 || size.height == 0) {
		p.data_size = 0;
		p.size = UVec2{0, 0};
		return;
	} else if (size == old_size) {
		return;
	}

	auto& allocator = memory::default_allocator();
	u8* data = p.data;
	unsigned const data_size = p.format.bytes_required(size);
	if (data_size > p.data_capacity) {
		data = static_cast<u8*>(allocator.allocate(data_size));
	}

	if (p.data_size > 0) {
		// No copy necessary when only changing the number of rows
		// within existing space
		if (data != p.data || size.width != old_size.width) {
			unsigned width  = min(size.width , old_size.width);
			unsigned height = min(size.height, old_size.height);
			pixmap::blit_direct_unscaled(
				p.format,
				data, size.width, UVec2{0, 0},
				p.data, old_size.width, UVec4{0, 0, width, height}
			);
		}
	}

	if (data != p.data) {
		allocator.deallocate(p.data);
		p.data = data;
		p.data_capacity = data_size;
	}
	p.data_size = data_size;
	p.size = size;

	// Fill new areas
	// new     size-   size+   width+  height+
	// |NNNN|  |...X|  |...V|  |...V|  |....|
	// |NNNN|  |...X|  |...V|  |...V|  |....|
	// |NNNN|  |XXXX|  |HHHH|  |...V|  |HHHH|
	UVec4 rect{};
	if (old_size.width == 0) {
		rect.x = 0;
		rect.y = 0;
		rect.width = size.width;
		rect.height = size.height;
		pixmap::fill(p, fill_color, rect);
		return;
	}
	if (size.width > old_size.width) {
		rect.x = old_size.width;
		rect.y = 0;
		rect.width = size.width - old_size.width;
		rect.height = size.height;
		if (size.height > old_size.height) {
			rect.height -= size.height - old_size.height;
		}
		pixmap::fill(p, fill_color, rect);
	}
	if (size.height > old_size.height) {
		rect.x = 0;
		rect.y = old_size.height;
		rect.width = size.width;
		rect.height = size.height - old_size.height;
		pixmap::fill(p, fill_color, rect);
	}
}

/// Free data.
void pixmap::free(Pixmap& p) {
	if (p.data) {
		memory::default_allocator().deallocate(p.data);
		p.data = nullptr;
		p.data_size = 0;
		p.data_capacity = 0;
		p.size = UVec2{0, 0};
	}
}

/// Copy.
///
/// The pixel format of p is set to the pixel format of from; no pixel
/// conversion is done.
void pixmap::copy(Pixmap& p, Pixmap const& from) {
	if (p.data_capacity < from.data_size) {
		p.data_capacity = from.data_size;
		auto& allocator = memory::default_allocator();
		allocator.deallocate(p.data);
		p.data = static_cast<u8*>(allocator.allocate(p.data_size));
	}
	p.format = from.format;
	p.data_size = from.data_size;
	p.size = from.size;
	if (from.data) {
		std::memcpy(p.data, from.data, p.data_size);
	}
}

/// Fill rectangle with color.
void pixmap::fill(Pixmap& p, Color color, UVec4 rect) {
	/*TOGO_DEBUG_LOGF(
		"fill: size = {%u, %u}, "
		"rect = {%2u, %2u,  %2u, %2u}\n",
		p.size.width, p.size.height,
		rect.x, rect.y, rect.width, rect.height
	);*/
	if (p.data_size == 0 || rect.x >= p.size.width || rect.y >= p.size.height) {
		return;
	}
	rect.width = min(p.size.width - rect.x, rect.width);
	rect.height = min(p.size.height - rect.y, rect.height);
	/*TOGO_DEBUG_LOGF(
		"    clipped = {%2u, %2u,  %2u, %2u}\n",
		rect.x, rect.y, rect.width, rect.height
	);*/
	if (rect.width == 0 || rect.height == 0) {
		return;
	}
	switch (p.format.data_type()) {
	case PixelDataType::c8: pixmap::fill_c8(p, color, rect); return;
	case PixelDataType::p32: pixmap::fill_p32(p, color, rect); return;
	}
}

} // namespace togo
