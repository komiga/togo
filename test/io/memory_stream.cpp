
#include <togo/error/assert.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/array.hpp>
#include <togo/io/io.hpp>
#include <togo/io/memory_stream.hpp>

#include "../common/helpers.hpp"
#include "./common.hpp"

using namespace togo;

signed main() {
	memory_init();

	MemoryStream stream{memory::default_allocator(), 0};
	test_writer(stream, true);
	TOGO_ASSERTE(io::seek_to(stream, 0) == 0);
	test_reader(stream, true);

	MemoryReader stream_const{array::begin(stream.buffer()), array::size(stream.buffer())};
	test_reader(stream_const, true);
	return 0;
}
