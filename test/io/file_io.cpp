
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
	u32 array[10];


	FileWriter writer;
	TOGO_ASSERTE(writer.open("data/file_stream.bin", false));
	io::write_value(writer, u8 {8});
	io::write_value(writer, u16{16});
	io::write_value(writer, u32{32});
	io::write_value(writer, u64{64});
	io::write_value(writer, float{-32.0f});

	u32 bit = 128;
	for (u32& v : array) {
		v = bit;
		bit *= 2;
	}
	io::write_array(writer, array, array_extent(array));
	writer.close();

	FileReader reader;
	TOGO_ASSERTE(reader.open("data/file_stream.bin"));
	TOGO_ASSERTE(io::read_value<u8 >(reader) ==  8);
	TOGO_ASSERTE(io::read_value<u16>(reader) == 16);
	TOGO_ASSERTE(io::read_value<u32>(reader) == 32);
	TOGO_ASSERTE(io::read_value<u64>(reader) == 64);
	TOGO_ASSERTE(float_exact(io::read_value<float>(reader), -32.0f));

	for (u32& v : array) {
		v = 0;
	}
	io::read_array(reader, array, array_extent(array));
	bit = 128;
	for (u32& v : array) {
		TOGO_ASSERTE(v == bit);
		bit *= 2;
	}
	reader.close();

	return 0;
}
