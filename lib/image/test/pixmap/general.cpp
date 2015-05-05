
#include <togo/image/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/image/pixmap/pixmap.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main() {
	memory_init();

	Color c_test1{0xAA, 0xBB, 0xCC, 0xDD};
	Color c_test2{0xDD, 0xCC, 0xBB, 0xAA};

	Pixmap p{PixelFormatID::rgba8};
	pixmap::resize(p, UVec2{1, 1});
	TOGO_ASSERTE(p.data && p.data_size == p.data_capacity && p.data_size == 1*1 * 4);
	auto data = reinterpret_cast<u32*>(p.data);
	data[0] = c_test1.packed();

	pixmap::resize(p, UVec2{4, 4}, c_test2);
	TOGO_ASSERTE(p.data && p.data_size == p.data_capacity && p.data_size == 4*4 * 4);
	data = reinterpret_cast<u32*>(p.data);
	TOGO_ASSERTE(data[0] == c_test1.packed());

	{auto end = pointer_add(data, p.data_size);
	auto c_test2_packed = c_test2.packed();
	for (auto check = data + 1; check != end; ++check) {
		TOGO_ASSERTF(
			*check == c_test2_packed,
			"%08x != %08x",
			*check, c_test2_packed
		);
	}}

	return 0;
}
