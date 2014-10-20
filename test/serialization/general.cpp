
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/fixed_array.hpp>
#include <togo/array.hpp>
#include <togo/string.hpp>
#include <togo/system.hpp>
#include <togo/random.hpp>
#include <togo/io.hpp>
#include <togo/memory_io.hpp>
#include <togo/serializer.hpp>
#include <togo/serialization/support.hpp>
#include <togo/serialization/fixed_array.hpp>
#include <togo/serialization/array.hpp>
#include <togo/binary_serializer.hpp>

#include "../common/helpers.hpp"

using namespace togo;

signed main() {
	memory_init();

	u64 const seed = system::secs_since_epoch();
	XS128A rng{seed};
	TOGO_LOGF("RNG seed: %ld\n", seed);

	MemoryStream stream{memory::default_allocator(), 2048};
	BinaryOutputSerializer oser{stream};
	BinaryInputSerializer iser{stream};

	// Primitives
	{
		u64 const basis = rng_next(rng);
		oser % basis;
		io::seek_to(stream, 0);

		u64 value = 0;
		iser % value;
		TOGO_ASSERTE(value == basis);
	}
	stream.clear();

	// SerBuffer
	{
		u64 const basis = rng_next(rng);
		oser % make_ser_buffer(&basis, sizeof(u64));
		io::seek_to(stream, 0);

		u64 value = 0;
		iser % make_ser_buffer(&value, sizeof(u64));
		TOGO_ASSERTE(value == basis);
	}
	stream.clear();

	// FixedArray
	{
		enum : unsigned { SIZE = 128 };
		FixedArray<u64, SIZE> basis{};
		fixed_array::resize(basis, SIZE);
		for (auto& value : basis) {
			value = rng_next(rng);
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
			value = rng_next(rng);
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

	return 0;
}
