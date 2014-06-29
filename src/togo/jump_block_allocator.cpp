#line 2 "togo/jump_block_allocator.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/utility.hpp>
#include <togo/log.hpp>
#include <togo/jump_block_allocator.hpp>

namespace togo {

namespace {
	static constexpr u32 const
	BLOCK_SIZE_MIN = 4 * 1024; // 4KB
}

#define BLOCK_NEXT_PTR(block) \
	*reinterpret_cast<void**>(block)

JumpBlockAllocator::~JumpBlockAllocator() {
	void* p = BLOCK_NEXT_PTR(_base_block);
	while (p) {
		TOGO_LOG_DEBUGF("JumpBlockAllocator: destroying block: %p\n", p);
		void* const next = BLOCK_NEXT_PTR(p);
		_fallback_allocator.deallocate(p);
		p = next;
	}
}

JumpBlockAllocator::JumpBlockAllocator(
	char* const base_block_begin,
	char* const base_block_end,
	Allocator& fallback_allocator
)
	: _base_block(base_block_begin)
	, _block_begin(base_block_begin)
	, _block_end(base_block_end)
	, _put(_base_block + sizeof(void*))
	, _fallback_allocator(fallback_allocator)
{
	BLOCK_NEXT_PTR(_block_begin) = nullptr;
}

void* JumpBlockAllocator::allocate(u32 const size, u32 const align) {
	_put = pointer_align(_put, align);
	if (signed_cast(size) > _block_end - _put) {
		u32 const block_size = max(
			u32{sizeof(void*)} + size + align,
			BLOCK_SIZE_MIN
		);
		_put = static_cast<char*>(_fallback_allocator.allocate(block_size));
		TOGO_LOG_DEBUGF("JumpBlockAllocator: new block: %u @ %p\n", block_size, _put);
		BLOCK_NEXT_PTR(_block_begin) = _put;
		_block_begin = _put;
		BLOCK_NEXT_PTR(_block_begin) = nullptr;
		_block_end = _block_begin + block_size;
		_put += sizeof(void*);
		_put = pointer_align(_put, align);
	}
	void* const p = _put;
	_put += size;
	return p;
}

} // namespace togo
