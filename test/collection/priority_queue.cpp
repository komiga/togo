
#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/priority_queue.hpp>

#include "../common/helpers.hpp"

using namespace togo;

#define PQ_ASSERTIONS(a, _size, _capacity) \
	TOGO_ASSERTE(priority_queue::size(a) == _size); \
	TOGO_ASSERTE(priority_queue::capacity(a) == _capacity); \
	TOGO_ASSERTE(priority_queue::space(a) == (_capacity - _size)); \
	TOGO_ASSERTE(priority_queue::empty(a) == (_size == 0)); \
	TOGO_ASSERTE(priority_queue::any(a) == (_size > 0))
//

signed main() {
	memory_init();

	TOGO_LOGF("sizeof(PriorityQueue<u32>) = %zu\n", sizeof(PriorityQueue<u32>));
	TOGO_LOGF("alignof(PriorityQueue<u32>) = %zu\n", alignof(PriorityQueue<u32>));

	auto const& less_func = less<u32>;
	{
		// Invariants
		PriorityQueue<u32> pq{less_func, memory::default_allocator()};
		PQ_ASSERTIONS(pq, 0, 0);

		priority_queue::push(pq, 42u);
		PQ_ASSERTIONS(pq, 1, 8);

		priority_queue::clear(pq);
		PQ_ASSERTIONS(pq, 0, 8);

		// Insertion
		u32 count = 10;
		while (count--) {
			priority_queue::push(pq, static_cast<u32>(10 - count));
			TOGO_ASSERTE(priority_queue::front(pq) == priority_queue::size(pq));
		}
		PQ_ASSERTIONS(pq, 10, 24);

		// Access
		TOGO_LOG("unordered iteration: ");
		for (u32 i = 0; i < priority_queue::size(pq); ++i) {
			TOGO_LOGF("%d ", pq[i]);
		}
		TOGO_LOG("\n");

		// Copy
		PriorityQueue<u32> copy{less_func, memory::default_allocator()};

		priority_queue::copy(copy, pq);
		TOGO_ASSERTE(priority_queue::size(copy) == priority_queue::size(pq));
		for (u32 i = 0; i < priority_queue::size(copy); ++i) {
			TOGO_ASSERTE(copy[i] == pq[i]);
		}

		// Unbroken removal
		TOGO_LOG("max order (pop): ");
		count = 10;
		while (count > 5) {
			TOGO_ASSERTE(priority_queue::front(pq) == count);
			TOGO_LOGF("%d ", priority_queue::front(pq));
			priority_queue::pop(pq);
			--count;
		}
		PQ_ASSERTIONS(pq, 5, 24);

		while (count--) {
			TOGO_LOGF("%d ", priority_queue::front(pq));
			priority_queue::pop(pq);
		}
		TOGO_LOG("\n");
		PQ_ASSERTIONS(pq, 0, 24);

		// Reinsertion
		count = 10;
		while (count--) {
			priority_queue::push(pq, count);
			TOGO_ASSERTE(priority_queue::front(pq) == 9);
		}
		PQ_ASSERTIONS(pq, 10, 24);
	}

	{
		PriorityQueue<u32> pq{less_func, memory::default_allocator()};
		u32 count = 100;
		u32 value = 1;
		u32 front = 0;

		// All even numbers removed
		while (count--) {
			priority_queue::push(pq, value++);
			priority_queue::push(pq, value++);
			front += 2;
			TOGO_ASSERTE(priority_queue::front(pq) == front);
			priority_queue::pop(pq);
		}

		// Remove all odd numbers
		front = 199;
		while (priority_queue::any(pq)) {
			TOGO_ASSERTE(priority_queue::front(pq) == front);
			priority_queue::pop(pq);
			front -= 2;
		}
		TOGO_ASSERTE(priority_queue::empty(pq));
		TOGO_ASSERTE(priority_queue::size(pq) == 0);
		TOGO_ASSERTE(priority_queue::space(pq) != 0);
	}

	return 0;
}
