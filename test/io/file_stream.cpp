
#include <togo/string/string.hpp>
#include <togo/io/file_stream.hpp>

#include "./common.hpp"

using namespace togo;

signed main() {
	static constexpr StringRef const path{"data/file_stream.bin"};
	{
		FileWriter writer;
		TOGO_ASSERTE(!writer.is_open());
		TOGO_ASSERTE(writer.open(path, false));
		TOGO_ASSERTE(writer.is_open());
		test_writer(writer, true);
		writer.close();
	}
	{
		FileReader reader;
		TOGO_ASSERTE(!reader.is_open());
		TOGO_ASSERTE(reader.open(path));
		TOGO_ASSERTE(reader.is_open());
		test_reader(reader, true);
		reader.close();
	}
	return 0;
}
