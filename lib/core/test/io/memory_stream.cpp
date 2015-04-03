
#include <togo/core/error/assert.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/memory_stream.hpp>

#include <togo-test/helpers.hpp>
#include "./common.hpp"

using namespace togo;

signed main() {
	memory_init();

	MemoryStream stream{memory::default_allocator(), 0};
	test_writer(stream, true);
	TOGO_ASSERTE(io::seek_to(stream, 0) == 0);
	test_reader(stream, true);

	MemoryReader stream_const{array::begin(stream.data()), stream.size()};
	test_reader(stream_const, true);
	return 0;
}
