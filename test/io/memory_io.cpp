
#include <togo/assert.hpp>
#include <togo/io.hpp>
#include <togo/memory.hpp>
#include <togo/memory_io.hpp>

#include "../common/helpers.hpp"
#include "./common.hpp"

using namespace togo;

signed
main() {
	core_init();

	MemoryStream stream{memory::default_allocator(), 0};
	test_writer(stream, true);
	TOGO_ASSERTE(io::seek_to(stream, 0) == 0);
	test_reader(stream, true);
	return 0;
}
