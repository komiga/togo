
#pragma once

#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/proto.hpp>

#include <cstring>

using namespace togo;

namespace {

union FloatInt32 {
	f32 f;
	u32 i;
};

union FloatInt64 {
	f64 f;
	u64 i;
};

bool float_exact_32(f32 const x, f32 const y) {
	FloatInt32 const xu{x};
	FloatInt32 const yu{y};
	return xu.i == yu.i;
}

bool float_exact_64(f64 const x, f64 const y) {
	FloatInt64 const xu{x};
	FloatInt64 const yu{y};
	return xu.i == yu.i;
}

static u8 const partial_out[5]{0, 8, 16, 32, 64};

struct TestStreamState {
	u8  i8 {8};
	u16 i16{16};
	u32 i32{32};
	u64 i64{64};
	f32 f32{-32.0f};
	f64 f64{-64.0f};
	u32 array[10]{0};
};

enum : signed {
	SIZE_VALUES
		= sizeof(TestStreamState::i8)
		+ sizeof(TestStreamState::i16)
		+ sizeof(TestStreamState::i32)
		+ sizeof(TestStreamState::i64)
		+ sizeof(TestStreamState::f32)
		+ sizeof(TestStreamState::f64)
	,
	SIZE_ARRAY = sizeof(TestStreamState::array),
	SIZE_BOTH = SIZE_VALUES + SIZE_ARRAY
};

} // anonymous namespace

void test_reader(IReader& stream, bool const seekable) {
	TestStreamState s{};
	std::memset(&s, 0, sizeof(TestStreamState));
	TOGO_ASSERTE(io::read_value(stream, s.i8 ) && s.i8  ==  8);
	TOGO_ASSERTE(io::read_value(stream, s.i16) && s.i16 == 16);
	TOGO_ASSERTE(io::read_value(stream, s.i32) && s.i32 == 32);
	TOGO_ASSERTE(io::read_value(stream, s.i64) && s.i64 == 64);
	TOGO_ASSERTE(io::read_value(stream, s.f32) && float_exact_32(s.f32, -32.0f));
	TOGO_ASSERTE(io::read_value(stream, s.f64) && float_exact_64(s.f64, -64.0f));

	if (seekable) {
		TOGO_ASSERTE(io::position(dynamic_cast<IStreamSeekable&>(stream)) == SIZE_VALUES);
	}

	TOGO_ASSERTE(io::read_array(stream, array_ref(s.array)));
	u32 bit = 1 << 7;
	for (u32& v : s.array) {
		TOGO_ASSERTE(v == bit);
		bit <<= 1;
	}

	if (seekable) {
		IStreamSeekable& stream_seekable = dynamic_cast<IStreamSeekable&>(stream);
		TOGO_ASSERTE(io::position(stream_seekable) == SIZE_BOTH);
		TOGO_ASSERTE(!io::status(stream_seekable).eof());

		TOGO_ASSERTE(io::seek_relative(stream_seekable, -SIZE_ARRAY) == SIZE_VALUES);
		TOGO_ASSERTE(io::position(stream_seekable) == SIZE_VALUES);
		TOGO_ASSERTE(io::seek_to(stream_seekable, 0) == 0);
		TOGO_ASSERTE(io::position(stream_seekable) == 0);

		TOGO_ASSERTE(io::seek_to(stream_seekable, SIZE_BOTH) == SIZE_BOTH);
		TOGO_ASSERTE(io::position(stream_seekable) == SIZE_BOTH);
	}

	u8 partial_in[10]{};
	unsigned read_size = 0;
	io::read(stream, partial_in, array_extent(partial_in), &read_size);
	TOGO_ASSERTE(!io::status(stream).fail() && io::status(stream).eof());
	TOGO_ASSERTE(read_size == array_extent(partial_out));
	TOGO_ASSERTE(std::memcmp(partial_in, partial_out, read_size) == 0);
}

void test_writer(IWriter& stream, bool const seekable) {
	TestStreamState s{};
	TOGO_ASSERTE(io::write_value(stream, s.i8 ));
	TOGO_ASSERTE(io::write_value(stream, s.i16));
	TOGO_ASSERTE(io::write_value(stream, s.i32));
	TOGO_ASSERTE(io::write_value(stream, s.i64));
	TOGO_ASSERTE(io::write_value(stream, s.f32));
	TOGO_ASSERTE(io::write_value(stream, s.f64));

	if (seekable) {
		TOGO_ASSERTE(io::position(dynamic_cast<IStreamSeekable&>(stream)) == SIZE_VALUES);
	}

	u32 bit = 1 << 7;
	for (u32& v : s.array) {
		v = bit;
		bit <<= 1;
	}
	TOGO_ASSERTE(io::write_array(stream, array_cref(s.array)));

	if (seekable) {
		IStreamSeekable& stream_seekable = dynamic_cast<IStreamSeekable&>(stream);
		TOGO_ASSERTE(io::position(stream_seekable) == SIZE_BOTH);
		TOGO_ASSERTE(!io::status(stream_seekable).eof());

		TOGO_ASSERTE(io::seek_relative(stream_seekable, -SIZE_ARRAY) == SIZE_VALUES);
		TOGO_ASSERTE(io::position(stream_seekable) == SIZE_VALUES);
		TOGO_ASSERTE(io::seek_to(stream_seekable, 0) == 0);
		TOGO_ASSERTE(io::position(stream_seekable) == 0);

		TOGO_ASSERTE(io::seek_to(stream_seekable, SIZE_BOTH) == SIZE_BOTH);
		TOGO_ASSERTE(io::position(stream_seekable) == SIZE_BOTH);
	}

	TOGO_ASSERTE(io::write(stream, partial_out, array_extent(partial_out)));
}
