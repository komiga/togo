
#include <togo/utility.hpp>
#include <togo/memory.hpp>
#include <togo/priority_queue.hpp>

#include "../common/helpers.hpp"

#include <cstdio>

using namespace togo;

#define PQ_ASSERTIONS(a, _size, _capacity, _empty) \
	TOGO_ASSERTE(priority_queue::size(a) == _size); \
	TOGO_ASSERTE(priority_queue::capacity(a) == _capacity); \
	TOGO_ASSERTE(priority_queue::space(a) == (_capacity - _size)); \
	TOGO_ASSERTE(priority_queue::empty(a) == _empty); \
	TOGO_ASSERTE(priority_queue::any(a) != _empty)
//

signed
main() {
	core_init();

	std::printf("sizeof(PriorityQueue<u32>) = %zu\n", sizeof(PriorityQueue<u32>));
	std::printf("alignof(PriorityQueue<u32>) = %zu\n", alignof(PriorityQueue<u32>));

	auto const& less_func = less<u32>;
	{
		// Invariants
		PriorityQueue<u32> pq{less_func, memory::default_allocator()};
		PQ_ASSERTIONS(pq, 0, 0, true);

		priority_queue::push(pq, 42u);
		PQ_ASSERTIONS(pq, 1, 8, false);

		priority_queue::clear(pq);
		PQ_ASSERTIONS(pq, 0, 8, true);

		// Insertion
		u32 count = 10;
		while (count--) {
			priority_queue::push(pq, static_cast<u32>(10 - count));
			TOGO_ASSERTE(priority_queue::front(pq) == priority_queue::size(pq));
		}
		PQ_ASSERTIONS(pq, 10, 24, false);

		// Access
		std::printf("unordered iteration: ");
		for (u32 i = 0; i < priority_queue::size(pq); ++i) {
			std::printf("%d ", pq[i]);
		}
		std::puts("");

		// Unbroken removal
		std::printf("max order (pop): ");
		count = 10;
		while (count > 5) {
			TOGO_ASSERTE(priority_queue::front(pq) == count);
			std::printf("%d ", priority_queue::front(pq));
			priority_queue::pop(pq);
			--count;
		}
		PQ_ASSERTIONS(pq, 5, 24, false);

		while (count--) {
			std::printf("%d ", priority_queue::front(pq));
			priority_queue::pop(pq);
		}
		std::puts("");
		PQ_ASSERTIONS(pq, 0, 24, true);

		// Reinsertion
		count = 10;
		while (count--) {
			priority_queue::push(pq, count);
			TOGO_ASSERTE(priority_queue::front(pq) == 9);
		}
		PQ_ASSERTIONS(pq, 10, 24, false);
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
