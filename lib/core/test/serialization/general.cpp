
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/collection/fixed_array.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/system/system.hpp>
#include <togo/core/random/random.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/memory_stream.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/core/serialization/fixed_array.hpp>
#include <togo/core/serialization/array.hpp>
#include <togo/core/serialization/string.hpp>

#include <togo/support/test.hpp>

using namespace togo;

signed main() {
	memory_init();

	u64 const seed = system::secs_since_epoch();
	XS128A rng{seed};
	TOGO_LOGF("RNG seed: %ld\n", seed);

	MemoryStream stream{memory::default_allocator(), 2048};
	BinaryOutputSerializer oser{stream};
	BinaryInputSerializer iser{stream};

	// Arithmetic
	{
		u64 const basis = random::next(rng);
		oser % basis;
		io::seek_to(stream, 0);

		u64 value = 0;
		iser % value;
		TOGO_ASSERTE(value == basis);
	}
	stream.clear();

	// SerBuffer
	{
		u64 const basis = random::next(rng);
		oser % make_ser_buffer(&basis, sizeof(u64));
		io::seek_to(stream, 0);

		u64 value = 0;
		iser % make_ser_buffer(&value, sizeof(u64));
		TOGO_ASSERTE(value == basis);
	}
	stream.clear();

	// Proxy
	{
		IntUDist<u32> udist{0, static_cast<u32>(~0)};
		u64 const basis = random::next_udist(rng, udist);
		oser % make_ser_proxy<u32>(basis);
		io::seek_to(stream, 0);

		u64 value = 0;
		iser % make_ser_proxy<u32>(value);
		TOGO_ASSERTE(value == basis);
	}
	stream.clear();

	// Enum (implicit proxy)
	{
		enum class E : u32 {
			I = 0,
			V = 0xCF9ABE2E,
		};
		E const basis = E::V;
		oser % basis;
		io::seek_to(stream, 0);

		E value = E::I;
		iser % value;
		TOGO_ASSERTE(value == basis);
	}
	stream.clear();

	// FixedArray
	{
		enum : unsigned { SIZE = 128 };
		FixedArray<u64, SIZE> basis{};
		fixed_array::resize(basis, SIZE);
		for (auto& value : basis) {
			value = random::next(rng);
		}
		oser % make_ser_collection<u8>(basis);
		io::seek_to(stream, 0);

		FixedArray<u64, SIZE> value{};
		iser % make_ser_collection<u8>(value);
		TOGO_ASSERTE(fixed_array::size(value) == SIZE);

		for (unsigned i = 0; i < SIZE; ++i) {
			TOGO_ASSERTE(value[i] == basis[i]);
		}
	}
	stream.clear();

	// Array
	{
		enum : unsigned { SIZE = 128 };
		Array<u64> basis{memory::default_allocator()};
		array::resize(basis, SIZE);
		for (auto& value : basis) {
			value = random::next(rng);
		}
		oser % make_ser_collection<u32>(basis);
		io::seek_to(stream, 0);

		Array<u64> value{memory::default_allocator()};
		iser % make_ser_collection<u32>(value);
		TOGO_ASSERTE(array::size(value) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i) {
			TOGO_ASSERTE(value[i] == basis[i]);
		}
	}
	stream.clear();

	// String (FixedArray<char, N>)
	{
		static constexpr char const BASIS[]{"value"};
		FixedArray<char, array_extent(BASIS)> basis{};
		string::copy(basis, BASIS);
		oser % make_ser_string<u32>(basis);
		io::seek_to(stream, 0);

		decltype(basis) value{};
		iser % make_ser_string<u32>(value);
		TOGO_ASSERTE(string::compare_equal(value, basis));
	}
	stream.clear();

	// String (char[N])
	{
		char basis[]{"value"};
		oser % make_ser_string<u32>(basis);
		io::seek_to(stream, 0);

		decltype(basis) value{};
		iser % make_ser_string<u32>(value);

		// NB: non-C-string StringRefs here OK due to
		// capacity == size of the strings
		TOGO_ASSERTE(string::compare_equal(value, basis));
	}
	stream.clear();

	return 0;
}
