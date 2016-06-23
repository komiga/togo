
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>

using namespace togo;

enum class E1 : unsigned {
	A = 1 << 0,
	B = 1 << 1,
	AB = A | B,
};

namespace togo {
	template<> struct enable_enum_bitwise_ops<E1> : true_type {};
}

struct Empty {};
struct NotEmpty {
	u8 x;
};

static_assert(sizeof_empty<Empty>() == 0, "");
static_assert(sizeof_empty<Empty&>() == 0, "");
static_assert(sizeof_empty<NotEmpty>() == sizeof(NotEmpty), "");
static_assert(sizeof_empty<NotEmpty&>() == sizeof(NotEmpty), "");

static_assert(num_params() == 0, "");
static_assert(num_params(1) == 1, "");
static_assert(num_params(NotEmpty{}) == 1, "");
static_assert(num_params(NotEmpty{}, NotEmpty{}) == 2, "");
static_assert(num_params(E1::A, NotEmpty{}, NotEmpty{}) == 3, "");

signed main() {
	E1 e = E1::A & E1::A;
	TOGO_ASSERTE(e == E1::A);
	e |= E1::B;
	TOGO_ASSERTE(e == E1::AB);
	e &= E1::B;
	TOGO_ASSERTE(e == E1::B);

	TOGO_ASSERTE(less(1, 2));
	TOGO_ASSERTE(!less(2, 1));
	TOGO_ASSERTE(!less(1, 1));

	TOGO_ASSERTE(!greater(1, 2));
	TOGO_ASSERTE(greater(2, 1));
	TOGO_ASSERTE(!greater(1, 1));

	unsigned x = 42, y = 3;
	swap(x, y);
	TOGO_ASSERTE(x == 3 && y == 42);

	unsigned a[4];
	static_assert(array_extent(a) == 4, "");

	TOGO_ASSERTE(min(1, 0) == 0 && min(0, 1) == 0);
	TOGO_ASSERTE(max(1, 0) == 1 && max(0, 1) == 1);
	TOGO_ASSERTE(clamp(+1, 0, 2) == 1);
	TOGO_ASSERTE(clamp(-2, 0, 2) == 0);
	TOGO_ASSERTE(clamp(+4, 0, 2) == 2);

	unsigned* p = &x;
	TOGO_ASSERTE(pointer_add(p, sizeof(x)) == (p + 1));

	return 0;
}
