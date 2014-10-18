#line 2 "togo/memory.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Memory interface.
@ingroup memory
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/memory_types.hpp>

#include <type_traits>
#include <new>
#include <cstdlib>

namespace togo {

/**
	@addtogroup memory
	@{
*/

namespace {
	template<class T>
	using pointer_type = typename std::remove_const<
		typename std::remove_pointer<T>::type
	>::type;
}

/// Construct an object with an allocator.
#define TOGO_CONSTRUCT(a, T, ...) \
	(new ((a).allocate(sizeof(T), alignof(T))) T(__VA_ARGS__))

/// Construct an object with an allocator and no alignment.
#define TOGO_CONSTRUCT_NA(a, T, ...) \
	(new ((a).allocate(sizeof(T), 0)) T(__VA_ARGS__))

/// Construct an object with an allocator and no initialization parameters.
/// This calls the default constructor.
#define TOGO_CONSTRUCT_DEFAULT(a, T) \
	(new ((a).allocate(sizeof(T), alignof(T))) T())

/// Construct an object with an allocator, no initialization parameters,
/// and no alignment.
///
/// This calls the default constructor.
#define TOGO_CONSTRUCT_DEFAULT_NA(a, T) \
	(new ((a).allocate(sizeof(T), 0)) T())

/// Destroy an object with an allocator.
///
/// Does nothing if p is nullptr. Calls destructor p->~T(); and
/// deallocates the object.
// Oh C++.
#define TOGO_DESTROY(a, p) \
	do { if (p) { \
		(p)->~pointer_type<decltype(p)>(); (a).deallocate(p); \
	} } while (false)

/// %Allocator base class.
class Allocator {
public:
	static constexpr u32 const
	/// Default allocation alignment.
	DEFAULT_ALIGNMENT = 4,
	/// Size value returned by unsupported operations.
	SIZE_NOT_TRACKED = static_cast<u32>(-1);

	Allocator() = default;
	Allocator(Allocator&&) = default;
	Allocator& operator=(Allocator&&) = default;

	Allocator(Allocator const&) = delete;
	Allocator& operator=(Allocator const&) = delete;

	/// Allocators should assert that they have no active allocations
	/// in the destructor.
	virtual ~Allocator() = 0;

	/// Get number of active allocations.
	virtual u32 num_allocations() const = 0;

	/// Get total number of bytes allocated by the allocator.
	///
	/// If an allocator does not support this operation, it shall
	/// return SIZE_NOT_TRACKED.
	virtual u32 total_size() const = 0;

	/// Get size of block allocated for p.
	///
	/// This may be greater than the actual size of the object type
	/// due to internal state and alignment. If an allocator does not
	/// support this operation, it shall return SIZE_NOT_TRACKED.
	virtual u32 allocation_size(void const* p) const = 0;

	/// Allocate a chunk of memory.
	///
	/// If align is 0, no alignment is used.
	virtual void* allocate(u32 size, u32 align = DEFAULT_ALIGNMENT) = 0;

	/// Deallocate a pointer.
	///
	/// Does nothing if p is nullptr.
	virtual void deallocate(void const* p) = 0;
};
inline Allocator::~Allocator() = default;

/** @} */ // end of doc-group memory

namespace memory {

/**
	@addtogroup memory
	@{
*/

static constexpr u32 const
	/// Minimum scratch size (8K).
	SCRATCH_SIZE_MINIMUM = 8 * 1024,
	/// Default scratch size (4MB).
	SCRATCH_SIZE_DEFAULT = 4 * 1024 * 1024
;

/// Initialize global allocators.
///
/// scratch_size is size of the scratch space block to pre-allocate.
/// If scratch_size < SCRATCH_SIZE_MINIMUM, an assertion will trigger.
void init(u32 const scratch_size = SCRATCH_SIZE_MINIMUM);

/// Shutdown allocators created by init().
void shutdown();

/// Get the default allocator.
///
/// This is a thread-safe growing heap allocator.
Allocator& default_allocator();

/// Get the scratch allocator.
///
/// This is a thread-safe allocator.
/// This should *only* be used for temporary memory. It uses a ring
/// buffer for allocations, backed by a block of memory from the
/// default allocator.
Allocator& scratch_allocator();

/** @} */ // end of doc-group memory

} // namespace memory

} // namespace togo
