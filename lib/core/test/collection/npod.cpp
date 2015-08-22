
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/array.hpp>

#include <togo/support/test.hpp>

using namespace togo;

struct S {
	unsigned* x;

	S()
		: x(TOGO_CONSTRUCT(memory::default_allocator(), unsigned, 0))
	{
		TOGO_LOGF("%p S(): %u\n", this, *x);
	}

	S(S const& other)
		: x(TOGO_CONSTRUCT(memory::default_allocator(), unsigned, *other.x))
	{
		TOGO_LOGF("%p S(S&&): %u\n", this, *x);
	}

	S(S&& other)
		: x(TOGO_CONSTRUCT(memory::default_allocator(), unsigned, *other.x))
	{
		TOGO_LOGF("%p S(S&&): %u\n", this, *x);
	}

	S(unsigned xx)
		: x(TOGO_CONSTRUCT(memory::default_allocator(), unsigned, xx))
	{
		TOGO_LOGF("%p S(x): %u\n", this, *x);
	}

	~S() {
		TOGO_LOGF("%p ~S(): %u\n", this, *x);
		memory::default_allocator().deallocate(x);
		x = nullptr;
	}

	S& operator=(S const& other) {
		*x = *other.x;
		return *this;
	}

	S& operator=(S&& other) {
		*x = *other.x;
		return *this;
	}
};

namespace togo {

template<> struct allow_collection_value_type<S> : true_type {};
template<> struct enable_collection_construction_and_destruction<S> : true_type {};

} // namespace togo

signed main() {
	memory_init();

	{
		Array<S> a{memory::default_allocator()};
		array::push_back_inplace(a);
	}
	{
		Array<S> a{memory::default_allocator()};
		array::resize(a, 5);
		array::set_capacity(a, 0);
	}
	{
		Array<S> a{memory::default_allocator()};
		array::push_back_inplace(a, 0);
		array::push_back_inplace(a, 1);
		array::push_back_inplace(a, 2);
		array::push_back_inplace(a, 3);
		array::push_back_inplace(a, 4);
		array::push_back_inplace(a, 5);

		{
		Array<S> b{memory::default_allocator()};
		array::copy(b, a);
		}

		array::pop_back(a);
		array::remove(a, 4);
		array::remove_over(a, 3);
		array::remove(a, &a[0]);
		array::remove_over(a, &a[0]);
		array::clear(a);
	}
	return 0;
}
