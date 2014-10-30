
#include <togo/error/assert.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/collection/queue.hpp>

#include "../common/helpers.hpp"

using namespace togo;

#define QUEUE_ASSERTIONS(a, _size, _capacity) \
	TOGO_ASSERTE(queue::size(a) == _size); \
	TOGO_ASSERTE(queue::capacity(a) == _capacity); \
	TOGO_ASSERTE(queue::space(a) == (_capacity - _size)); \
	TOGO_ASSERTE(queue::empty(a) == (_size == 0)); \
	TOGO_ASSERTE(queue::any(a) == (_size > 0))
//

signed main() {
	memory_init();

	TOGO_LOGF("sizeof(Queue<u32>) = %zu\n", sizeof(Queue<u32>));
	TOGO_LOGF("alignof(Queue<u32>) = %zu\n", alignof(Queue<u32>));

	{
		// Invariants
		Queue<u32> q{memory::default_allocator()};
		QUEUE_ASSERTIONS(q, 0, 0);

		queue::push_back(q, 42u);
		QUEUE_ASSERTIONS(q, 1, 8);

		queue::clear(q);
		QUEUE_ASSERTIONS(q, 0, 8);

		// Insertion
		u32 count = 10;
		while (count--) {
			queue::push_back(q, static_cast<u32>(10 - count));
		}
		QUEUE_ASSERTIONS(q, 10, 24);

		// Access
		TOGO_LOG("direct iteration order: ");
		for (u32 i = 0; i < queue::size(q); ++i) {
			TOGO_ASSERTE(q[i] == i + 1);
			TOGO_LOGF("%d ", q[i]);
		}
		TOGO_LOG("\n");

		// Copy
		Queue<u32> copy{memory::default_allocator()};

		queue::copy(copy, q);
		TOGO_ASSERTE(queue::size(copy) == queue::size(q));
		for (u32 i = 0; i < queue::size(copy); ++i) {
			TOGO_ASSERTE(copy[i] == q[i]);
		}

		// Unbroken FIFO removal
		TOGO_LOG("FIFO order (pop): ");
		count = 5;
		while (count--) {
			TOGO_LOGF("%d ", queue::front(q));
			queue::pop_front(q);
		}
		QUEUE_ASSERTIONS(q, 5, 24);

		count = 5;
		while (count--) {
			TOGO_LOGF("%d ", queue::front(q));
			queue::pop_front(q);
		}
		TOGO_LOG("\n");
		QUEUE_ASSERTIONS(q, 0, 24);

		// Reinsertion
		count = 10;
		while (count--) {
			queue::push_back(q, static_cast<u32>(10 - count));
		}
		QUEUE_ASSERTIONS(q, 10, 24);

		// Unbroken removal
		TOGO_LOG("LIFO order (pop): ");
		count = 5;
		while (count--) {
			TOGO_LOGF("%d ", queue::back(q));
			queue::pop_back(q);
		}
		QUEUE_ASSERTIONS(q, 5, 24);

		count = 5;
		while (count--) {
			TOGO_LOGF("%d ", queue::back(q));
			queue::pop_back(q);
		}
		TOGO_LOG("\n");
		QUEUE_ASSERTIONS(q, 0, 24);
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
