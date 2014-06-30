
#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/stream.hpp>
#include <togo/file_stream.hpp>

#include <cstdio>

using namespace togo;

signed
main() {
	FileWriter writer;
	TOGO_ASSERTE(writer.open("data/file_stream.bin", false));
	stream::write_value(writer, u8 {8});
	stream::write_value(writer, u16{16});
	stream::write_value(writer, u32{32});
	stream::write_value(writer, u64{64});
	writer.close();

	FileReader reader;
	TOGO_ASSERTE(reader.open("data/file_stream.bin"));
	TOGO_ASSERTE( 8 == stream::read_value<u8 >(reader));
	TOGO_ASSERTE(16 == stream::read_value<u16>(reader));
	TOGO_ASSERTE(32 == stream::read_value<u32>(reader));
	TOGO_ASSERTE(64 == stream::read_value<u64>(reader));
	reader.close();

	return 0;
}
