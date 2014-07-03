
#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/io.hpp>
#include <togo/file_io.hpp>

#include <cstdio>

using namespace togo;

union FloatInt {
	float f;
	u32 i;
};

bool float_exact(float const x, float const y) {
	FloatInt const xu{x};
	FloatInt const yu{y};
	return xu.i == yu.i;
}

signed
main() {
	static constexpr char const* const path = "data/file_stream.bin";
	u8  v8 {8};
	u16 v16{16};
	u32 v32{32};
	u64 v64{64};
	float vfloat{-32.0f};
	u32 array[10];

	enum : signed {
		SIZE_VALUES
			= sizeof(v8)
			+ sizeof(v16)
			+ sizeof(v32)
			+ sizeof(v64)
			+ sizeof(vfloat)
		,
		SIZE_ARRAY = sizeof(array),
		SIZE_BOTH = SIZE_VALUES + SIZE_ARRAY
	};

	{
		FileWriter writer;
		TOGO_ASSERTE(writer.open(path, false));
		TOGO_ASSERTE(io::write_value(writer, v8 ));
		TOGO_ASSERTE(io::write_value(writer, v16));
		TOGO_ASSERTE(io::write_value(writer, v32));
		TOGO_ASSERTE(io::write_value(writer, v64));
		TOGO_ASSERTE(io::write_value(writer, vfloat));
		TOGO_ASSERTE(io::position(writer) == SIZE_VALUES);

		u32 bit = 128;
		for (u32& v : array) {
			v = bit;
			bit <<= 1;
		}
		TOGO_ASSERTE(io::write_array(writer, array, array_extent(array)));
		TOGO_ASSERTE(io::position(writer) == SIZE_BOTH);
		TOGO_ASSERTE(!io::status(writer).eof());

		TOGO_ASSERTE(io::seek_relative(writer, -SIZE_ARRAY) == SIZE_VALUES);
		TOGO_ASSERTE(io::position(writer) == SIZE_VALUES);
		TOGO_ASSERTE(io::seek_to(writer, 0) == 0);
		TOGO_ASSERTE(io::position(writer) == 0);

		writer.close();
	}

	{
		FileReader reader;
		v8 = 0;
		v16 = 0;
		v32 = 0;
		v64 = 0;
		vfloat = 0.0f;
		TOGO_ASSERTE(reader.open(path));
		TOGO_ASSERTE(io::read_value(reader, v8 ) && v8  ==  8);
		TOGO_ASSERTE(io::read_value(reader, v16) && v16 == 16);
		TOGO_ASSERTE(io::read_value(reader, v32) && v32 == 32);
		TOGO_ASSERTE(io::read_value(reader, v64) && v64 == 64);
		TOGO_ASSERTE(io::read_value(reader, vfloat) && float_exact(vfloat, -32.0f));

		for (u32& v : array) {
			v = 0;
		}
		TOGO_ASSERTE(io::read_array(reader, array, array_extent(array)));
		TOGO_ASSERTE(io::position(reader) == SIZE_BOTH);
		TOGO_ASSERTE(!io::status(reader).eof());

		TOGO_ASSERTE(io::seek_relative(reader, -SIZE_ARRAY) == SIZE_VALUES);
		TOGO_ASSERTE(io::position(reader) == SIZE_VALUES);
		TOGO_ASSERTE(io::seek_to(reader, 0) == 0);
		TOGO_ASSERTE(io::position(reader) == 0);

		u32 bit = 128;
		for (u32& v : array) {
			TOGO_ASSERTE(v == bit);
			bit <<= 1;
		}
		reader.close();
	}

	return 0;
}
