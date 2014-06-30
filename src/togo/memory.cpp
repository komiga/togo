#line 2 "togo/memory.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/mutex.hpp>
#include <togo/memory.hpp>
#include <togo/assert.hpp>
#include <togo/external/dlmalloc_import.hpp>

#include <new>
#include <cstdio>

// TODO: ScratchAllocator: circular buffer temp allocator backed
// by default_allocator

// TODO: HeapAllocator: dlmalloc doesn't give us an accurate measure
// of the size allocated in total (due to tracking). Track manually?
// If so, we should pre-calculate the size using the same formula
// dlmalloc does *instead* of making an extra mspace_usable_size()
// call after every allocation.

namespace togo {

namespace {

class HeapAllocator
	: public Allocator
{
private:
	mspace _mspace;
	u32 _num_allocations;
	Mutex _op_mutex;

public:
	HeapAllocator(HeapAllocator&&) = default;
	HeapAllocator& operator=(HeapAllocator&&) = default;

	HeapAllocator() = delete;
	HeapAllocator(HeapAllocator const&) = delete;
	HeapAllocator& operator=(HeapAllocator const&) = delete;

	~HeapAllocator() override {
		TOGO_ASSERT(_num_allocations == 0, "allocator destroyed with active allocations");
		destroy_mspace(_mspace);
	}

	HeapAllocator(u32 const capacity)
		: _mspace(nullptr)
		, _num_allocations(0)
		, _op_mutex(MutexType::normal)
	{
		_mspace = create_mspace(capacity, 0);
		TOGO_ASSERT(_mspace, "failed to create mspace");
	}

	u32 num_allocations() const override {
		return _num_allocations;
	}

	u32 total_size() const override {
		MutexLock op_mutex_lock{const_cast<Mutex&>(_op_mutex)};
		return mspace_mallinfo(const_cast<void*>(_mspace)).uordblks;
	}

	u32 allocation_size(void const* const p) const override {
		MutexLock op_mutex_lock{const_cast<Mutex&>(_op_mutex)};
		size_t const size = mspace_usable_size(p);
		TOGO_DEBUG_ASSERT(size != 0, "attempted to access size for non-allocator pointer");
		return static_cast<u32>(size);
	}

	void* allocate(u32 const size, u32 const align) override {
		MutexLock op_mutex_lock{_op_mutex};

		void* p = nullptr;
		if (align != 0) {
			p = mspace_memalign(_mspace, align, size);
		} else {
			p = mspace_malloc(_mspace, size);
		}
		TOGO_ASSERTF(p, "allocation failed: size = %u, align = %u", size, align);
		++_num_allocations;
		/*std::printf("allocate: %u + %u = %u\n", size, align, size + align);
		auto const mi = mspace_mallinfo(const_cast<void*>(_mspace));
		std::printf(
			"  arena    = %zu\n"
			"  ordblks  = %zu\n"
			"  smblks   = %zu\n"
			"  hblks    = %zu\n"
			"  hblkhd   = %zu\n"
			"  usmblks  = %zu\n"
			"  fsmblks  = %zu\n"
			"  uordblks = %zu\n"
			"  fordblks = %zu\n"
			"  keepcost = %zu\n",
			mi.arena,
			mi.ordblks,
			mi.smblks,
			mi.hblks,
			mi.hblkhd,
			mi.usmblks,
			mi.fsmblks,
			mi.uordblks,
			mi.fordblks,
			mi.keepcost
		);*/
		return p;
	}

	void deallocate(void* p) override {
		if (p) {
			MutexLock op_mutex_lock{_op_mutex};
			mspace_free(_mspace, p);
			--_num_allocations;
		}
	}
};

} // anonymous namespace

namespace memory {

namespace {

using default_allocator_type = HeapAllocator;
//using scratch_allocator_type = ScratchAllocator;

struct MemoryGlobals {
	bool active{false};
	default_allocator_type* default_allocator{nullptr};
	//scratch_allocator_type* scratch_allocator{nullptr};

	static constexpr u32 const
	BUFFER_SIZE = sizeof(default_allocator_type)/* + sizeof(scratch_allocator_type)*/;
	char buffer[BUFFER_SIZE];
};
MemoryGlobals _mem_globals{};

static constexpr u32 const
HEAP_CAPACITY = 8 * 1024 * 1024; // 8MB

} // anonymous namespace

void init(u32 const scratch_size) {
	TOGO_ASSERT(!_mem_globals.active, "memory system has already been initialized");
	TOGO_ASSERT(
		scratch_size >= memory::SCRATCH_SIZE_MINIMUM,
		"scratch_size is below the minimum"
	);

	char* p = _mem_globals.buffer;
	_mem_globals.default_allocator = new (p) default_allocator_type(HEAP_CAPACITY);
	/*p += sizeof(default_allocator_type);
	_mem_globals.scratch_allocator = new (p) scratch_allocator_type(
		*_mem_globals.default_allocator,
		scratch_size
	);*/
	_mem_globals.active = true;
}

void shutdown() {
	TOGO_ASSERT(_mem_globals.active, "memory system has not been initialized");

	/*_mem_globals.scratch_allocator->~scratch_allocator_type();
	_mem_globals.scratch_allocator = nullptr;*/
	_mem_globals.default_allocator->~default_allocator_type();
	_mem_globals.default_allocator = nullptr;
	_mem_globals.active = false;
}

Allocator& default_allocator() {
	TOGO_DEBUG_ASSERT(_mem_globals.active, "memory system has not been initialized");
	return *_mem_globals.default_allocator;
}

Allocator& scratch_allocator() {
	TOGO_DEBUG_ASSERT(_mem_globals.active, "memory system has not been initialized");
	return *_mem_globals.default_allocator;
}

} // namespace memory

} // namespace togo
