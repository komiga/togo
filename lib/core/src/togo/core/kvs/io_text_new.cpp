#line 2 "togo/core/kvs/io_text_new.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/log/log.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/memory/temp_allocator.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/io/file_stream.hpp>
#include <togo/core/parser/types.hpp>
#include <togo/core/parser/parser.hpp>
#include <togo/core/parser/parse_state.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/kvs/io/parser_new.ipp>

#include <cstdio>

namespace togo {

/// Read text-format KVS from stream.
///
/// Returns false if a parser error occurred.
bool kvs::read_text_new(KVS& root, ArrayRef<char const> data, ParseError* error IGEN_DEFAULT(nullptr)) {
	TempAllocator<4096> allocator{};
	KVSParseState ud{0};
	ParseState state{
		allocator,
		error,
		&ud
	};
	array::reserve(state.results, (4096 / sizeof(ParseResult)) - sizeof(void*));
	kvs::set_type(root, KVSType::node);
	kvs::clear(root);
	set_data_array(state, data);
	push(state, {rtype_kvs, &root});
	bool success = parser::parse(p_root, state);
	return success;
}

} // namespace togo
