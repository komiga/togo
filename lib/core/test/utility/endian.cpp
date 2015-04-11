
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/utility/endian.hpp>
#include <togo/core/log/log.hpp>

using namespace togo;

template<class T, unsigned = sizeof(T)>
struct int_type;

template<class T> struct int_type<T, 2> { using type = u16; };
template<class T> struct int_type<T, 4> { using type = u32; };
template<class T> struct int_type<T, 8> { using type = u64; };

constexpr auto ENSURE_SWAP
	= (Endian::system == Endian::little)
	? Endian::big
	: Endian::little
;

#define IC(x) static_cast<u64>(reinterpret_cast<I const&>(x))

template<class T>
void do_test(T const value, T const check) {
	using I = typename int_type<T>::type;
	auto const a = reverse_bytes_copy(value);
	auto const b = reverse_bytes_copy_if(value, ENSURE_SWAP);
	auto const c = reverse_bytes_copy_if(value, Endian::system);

	u64 const up_value = IC(value);
	u64 const up_check = IC(check);
	u64 const reversed = IC(a);
	u64 const reversed_if = IC(b);
	u64 const reversed_no = IC(c);
	TOGO_LOGF("%0lx -> %0lx\n", up_value, reversed);
	TOGO_ASSERT(
		reversed == reversed_if && reversed == up_check,
		"something has gone deliciously wrong"
	);
	TOGO_ASSERT(reversed_no == up_value, "something has gone deliciously wrong");
}

static constexpr u64 const V64   = 0x1122334455667788;
static constexpr u64 const V64_R = 0x8877665544332211;

static constexpr u32 const V32   = 0x11223344;
static constexpr u32 const V32_R = 0x44332211;

static constexpr u16 const V16   = 0x1122;
static constexpr u16 const V16_R = 0x2211;

constexpr char const* const endian_name[]{"little", "big"};

signed main() {
	TOGO_LOGF("Endian::system = %s\n\n", endian_name[unsigned_cast(Endian::system)]);

	auto const x = V16;
	u8 const* const x_bytes = reinterpret_cast<u8 const*>(&x);

	Endian endian;
	if (x_bytes[0] == (x & 0xFF)) {
		endian = Endian::little;
	} else {
		endian = Endian::big;
	}
	TOGO_ASSERT(endian == Endian::system, "togo is lying");

	do_test(V16, V16_R);
	do_test(V32, V32_R);
	do_test(V64, V64_R);
	do_test(reinterpret_cast<f32 const&>(V32), reinterpret_cast<f32 const&>(V32_R));
	do_test(reinterpret_cast<f64 const&>(V64), reinterpret_cast<f64 const&>(V64_R));
	return 0;
}
