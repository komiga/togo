
#include <togo/types.hpp>
#include <togo/assert.hpp>
#include <togo/memory.hpp>
#include <togo/object_buffer.hpp>

#include "../common/helpers.hpp"

using namespace togo;

enum class ObjectType : unsigned {
	Empty,
	X1,
	X2,
};

struct Empty {};

struct X1 {
	u32 value;
};

struct X2 {
	u64 value;
};

template<class T, class S>
void test_object_buffer(Allocator& allocator, u32 const init_capacity) {
	ObjectBuffer<T, S> ob{allocator, init_capacity};

	TOGO_ASSERTE(object_buffer::size(ob) == 0 && object_buffer::empty(ob));
	TOGO_ASSERTE(!object_buffer::has_more(ob));

	object_buffer::write(ob, ObjectType::Empty, Empty{});
	object_buffer::write_empty(ob, ObjectType::X1);
	object_buffer::write(ob, ObjectType::X1, X1{32});
	object_buffer::write(ob, ObjectType::X2, X2{64});
	TOGO_ASSERTE(object_buffer::size(ob) == 4 && !object_buffer::empty(ob));

	object_buffer::begin_consume(ob);
	{
		ObjectType type{};
		void const* object = nullptr;

		TOGO_ASSERTE(object_buffer::has_more(ob));
		TOGO_ASSERTE(object_buffer::read(ob, type, object) == 0);
		TOGO_ASSERTE(type == ObjectType::Empty);
		TOGO_ASSERTE(object == nullptr);

		TOGO_ASSERTE(object_buffer::has_more(ob));
		TOGO_ASSERTE(object_buffer::read(ob, type, object) == 0);
		TOGO_ASSERTE(type == ObjectType::X1);
		TOGO_ASSERTE(object == nullptr);

		TOGO_ASSERTE(object_buffer::has_more(ob));
		unsigned const size = object_buffer::read(ob, type, object);
		TOGO_ASSERTE(size == sizeof(X1));
		TOGO_ASSERTE(type == ObjectType::X1);
		TOGO_ASSERTE(object != nullptr);
		TOGO_ASSERTE(static_cast<X1 const*>(object)->value == 32);

		TOGO_ASSERTE(object_buffer::has_more(ob));
		TOGO_ASSERTE(object_buffer::read(ob, type, object) == sizeof(X2));
		TOGO_ASSERTE(type == ObjectType::X2);
		TOGO_ASSERTE(object != nullptr);
		TOGO_ASSERTE(static_cast<X2 const*>(object)->value == 64);

		TOGO_ASSERTE(!object_buffer::has_more(ob));
	}
	object_buffer::end_consume(ob);
	TOGO_ASSERTE(object_buffer::size(ob) == 0 && object_buffer::empty(ob));

	object_buffer::write(ob, ObjectType::X2, X2{64});
	TOGO_ASSERTE(object_buffer::size(ob) == 1);
	object_buffer::clear(ob);
	TOGO_ASSERTE(object_buffer::size(ob) == 0 && object_buffer::empty(ob));
}

signed
main() {
	memory_init();

	test_object_buffer<u8, u8>(memory::default_allocator(), 0);
	test_object_buffer<u32, u16>(memory::default_allocator(), 0);
	test_object_buffer<u32, u64>(memory::default_allocator(), 0);
	return 0;
}
