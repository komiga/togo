
#include <togo/file_io.hpp>

#include "./common.hpp"

using namespace togo;

signed
main() {
	static constexpr char const* const path = "data/file_stream.bin";
	{
		FileWriter writer;
		TOGO_ASSERTE(writer.open(path, false));
		test_writer(writer, true);
		writer.close();
	}
	{
		FileReader reader;
		TOGO_ASSERTE(reader.open(path));
		test_reader(reader, true);
		reader.close();
	}
	return 0;
}
