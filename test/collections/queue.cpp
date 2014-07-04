
#include <togo/memory.hpp>
#include <togo/queue.hpp>

#include "../common/helpers.hpp"

#include <cstdio>

using namespace togo;

#define QUEUE_ASSERTIONS(a, _size, _capacity, _empty) \
	TOGO_ASSERTE(queue::size(a) == _size); \
	TOGO_ASSERTE(queue::capacity(a) == _capacity); \
	TOGO_ASSERTE(queue::space(a) == (_capacity - _size)); \
	TOGO_ASSERTE(queue::empty(a) == _empty); \
	TOGO_ASSERTE(queue::any(a) != _empty)
//

signed
main() {
	core_init();

	std::printf("sizeof(Queue<u32>) = %zu\n", sizeof(Queue<u32>));
	std::printf("alignof(Queue<u32>) = %zu\n", alignof(Queue<u32>));

	{
		// Invariants
		Queue<u32> q{memory::default_allocator()};
		QUEUE_ASSERTIONS(q, 0, 0, true);

		queue::push_back(q, 42u);
		QUEUE_ASSERTIONS(q, 1, 8, false);

		queue::clear(q);
		QUEUE_ASSERTIONS(q, 0, 8, true);

		// Insertion
		u32 count = 10;
		while (count--) {
			queue::push_back(q, static_cast<u32>(10 - count));
		}
		QUEUE_ASSERTIONS(q, 10, 24, false);

		// Access
		std::printf("direct iteration order: ");
		for (u32 i = 0; i < queue::size(q); ++i) {
			TOGO_ASSERTE(q[i] == i + 1);
			std::printf("%d ", q[i]);
		}
		std::puts("");

		// Unbroken FIFO removal
		std::printf("FIFO order (pop): ");
		count = 5;
		while (count--) {
			std::printf("%d ", queue::front(q));
			queue::pop_front(q);
		}
		QUEUE_ASSERTIONS(q, 5, 24, false);

		count = 5;
		while (count--) {
			std::printf("%d ", queue::front(q));
			queue::pop_front(q);
		}
		std::puts("");
		QUEUE_ASSERTIONS(q, 0, 24, true);

		// Reinsertion
		count = 10;
		while (count--) {
			queue::push_back(q, static_cast<u32>(10 - count));
		}
		QUEUE_ASSERTIONS(q, 10, 24, false);

		// Unbroken removal
		std::printf("LIFO order (pop): ");
		count = 5;
		while (count--) {
			std::printf("%d ", queue::back(q));
			queue::pop_back(q);
		}
		QUEUE_ASSERTIONS(q, 5, 24, false);

		count = 5;
		while (count--) {
			std::printf("%d ", queue::back(q));
			queue::pop_back(q);
		}
		std::puts("");
		QUEUE_ASSERTIONS(q, 0, 24, true);
	}

	{
		Queue<u32> q{memory::default_allocator()};

		u32 value = 1;
		queue::push_back(q, value++);
		TOGO_ASSERTE(queue::front(q) == 1);

		queue::push_back(q, value++);
		TOGO_ASSERTE(queue::front(q) == 1);
		queue::pop_front(q);
		TOGO_ASSERTE(queue::front(q) == 2);

		queue::push_back(q, value++);
		TOGO_ASSERTE(queue::front(q) == 2);
		queue::pop_front(q);
		TOGO_ASSERTE(queue::front(q) == 3);
	}

	{
		Queue<u32> q{memory::default_allocator()};
		u32 count = 100;
		u32 value = 0;
		u32 front = 0;
		while (count--) {
			queue::push_back(q, value++);
			queue::push_back(q, value++);
			TOGO_ASSERTE(queue::front(q) == front);
			queue::pop_front(q);
			++front;
		}
		while (queue::any(q)) {
			TOGO_ASSERTE(queue::front(q) == front);
			queue::pop_front(q);
			++front;
		}
		TOGO_ASSERTE(queue::empty(q));
		TOGO_ASSERTE(queue::size(q) == 0);
		TOGO_ASSERTE(queue::space(q) != 0);
	}

	return 0;
}