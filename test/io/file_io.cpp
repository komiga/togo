
#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/io.hpp>
#include <togo/file_io.hpp>

#include <cstdio>

using namespace togo;

signed
main() {
	FileWriter writer;
	TOGO_ASSERTE(writer.open("data/file_stream.bin", false));
	io::write_value(writer, u8 {8});
	io::write_value(writer, u16{16});
	io::write_value(writer, u32{32});
	io::write_value(writer, u64{64});
	writer.close();

	FileReader reader;
	TOGO_ASSERTE(reader.open("data/file_stream.bin"));
	TOGO_ASSERTE( 8 == io::read_value<u8 >(reader));
	TOGO_ASSERTE(16 == io::read_value<u16>(reader));
	TOGO_ASSERTE(32 == io::read_value<u32>(reader));
	TOGO_ASSERTE(64 == io::read_value<u64>(reader));
	reader.close();

	return 0;
}
