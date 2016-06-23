#line 2 "togo/core/memory/fixed_allocator.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/fixed_allocator.hpp>

namespace togo {

void* memory::fixed_allocator_allocate(
	u8** put_pointer, u8* buffer_end,
	unsigned const size, unsigned const align
) {
	TOGO_ASSERTE(size != 0);
	u8* p = pointer_align(*put_pointer, align);
	TOGO_ASSERT((p + size) <= buffer_end, "allocator buffer overflow");
	*put_pointer = p + size;
	return p;
}

} // namespace togo
