
#pragma once

#include <togo/utility/utility.hpp>
#include <togo/error/assert.hpp>
#include <togo/io/types.hpp>
#include <togo/io/proto.hpp>
#include <togo/io/io.hpp>

#include <cstring>

using namespace togo;

namespace {

union FloatInt {
	float f;
	u32 i;
};

bool float_exact(float const x, float const y) {
	FloatInt const xu{x};
	FloatInt const yu{y};
	return xu.i == yu.i;
}

static u8 const partial_out[5]{0, 8, 16, 32, 64};

struct TestStreamState {
	u8  v8 {8};
	u16 v16{16};
	u32 v32{32};
	u64 v64{64};
	float vfloat{-32.0f};
	u32 array[10]{0};
};

enum : signed {
	SIZE_VALUES
		= sizeof(TestStreamState::v8)
		+ sizeof(TestStreamState::v16)
		+ sizeof(TestStreamState::v32)
		+ sizeof(TestStreamState::v64)
		+ sizeof(TestStreamState::vfloat)
	,
	SIZE_ARRAY = sizeof(TestStreamState::array),
	SIZE_BOTH = SIZE_VALUES + SIZE_ARRAY
};

} // anonymous namespace

void test_reader(IReader& stream, bool const seekable) {
	TestStreamState s{};
	std::memset(&s, 0, sizeof(TestStreamState));
	TOGO_ASSERTE(io::read_value(stream, s.v8 ) && s.v8  ==  8);
	TOGO_ASSERTE(io::read_value(stream, s.v16) && s.v16 == 16);
	TOGO_ASSERTE(io::read_value(stream, s.v32) && s.v32 == 32);
	TOGO_ASSERTE(io::read_value(stream, s.v64) && s.v64 == 64);
	TOGO_ASSERTE(io::read_value(stream, s.vfloat) && float_exact(s.vfloat, -32.0f));

	if (seekable) {
		TOGO_ASSERTE(io::position(dynamic_cast<IStreamSeekable&>(stream)) == SIZE_VALUES);
	}

	TOGO_ASSERTE(io::read_array(stream, s.array, array_extent(&TestStreamState::array)));
	u32 bit = 128;
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
	TOGO_ASSERTE(io::write_value(stream, s.v8 ));
	TOGO_ASSERTE(io::write_value(stream, s.v16));
	TOGO_ASSERTE(io::write_value(stream, s.v32));
	TOGO_ASSERTE(io::write_value(stream, s.v64));
	TOGO_ASSERTE(io::write_value(stream, s.vfloat));

	if (seekable) {
		TOGO_ASSERTE(io::position(dynamic_cast<IStreamSeekable&>(stream)) == SIZE_VALUES);
	}

	u32 bit = 128;
	for (u32& v : s.array) {
		v = bit;
		bit <<= 1;
	}
	TOGO_ASSERTE(io::write_array(stream, s.array, array_extent(&TestStreamState::array)));

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
