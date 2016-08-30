#line 2 "togo/core/kvs/io_text.cpp"
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
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/kvs/io/parser.ipp>

#include <cstdio>

namespace togo {

/// Read text-format KVS from stream.
///
/// Returns false if a parser error occurred. pinfo will have the
/// position and error message of the parser.
bool kvs::read_text(KVS& root, IReader& stream, KVSParserInfo& pinfo) {
	TempAllocator<4096> allocator{};
	KVSParser p{
		root, stream, pinfo,
		{allocator},
		{allocator},
		1, 0, PC_EOF,
		Vec4{no_init_tag{}}, 0,
		PF_NONE,
		PS_NAME,
		PV_NONE
	};
	array::reserve(p.stack, 32);
	array::reserve(p.buffer, 4096 - (1 + 32) * sizeof(void*));
	kvs::set_type(p.root, KVSType::node);
	kvs::clear(p.root);
	parser_push(p, p.root);
	while (parser_next(p)) {
		if (!parser_skip_whitespace(p,
			p.stage == PS_NAME ||
			p.flags & PF_VALUE_NAMELESS
		)) {
			break;
		}
		switch (p.stage) {
		case PS_NAME: parser_stage_name(p); break;
		case PS_ASSIGN: parser_stage_assign(p); break;
		case PS_VALUE: parser_stage_value(p); break;
		}
		if (p.flags & PF_ERROR || p.c == PC_EOF) {
			break;
		}
	}
	if (p.flags & PF_ERROR) {
		// do nothing
	} else if (io::status(p.stream).fail()) {
		PARSER_ERROR(p, "stream read failure");
	} else if (p.c == PC_EOF) {
		if (p.flags & (PF_NAME | PF_ASSIGN)) {
			PARSER_ERROR(p, "expected value, got EOF");
		} else if (array::size(p.stack) > 1) {
			PARSER_ERRORF(p, "%u unclosed collection(s) at EOF", array::size(p.stack) - 1);
		}
	}
	return ~p.flags & PF_ERROR;
}

/// Read text-format KVS from stream (sans parser info).
bool kvs::read_text(KVS& root, IReader& stream) {
	KVSParserInfo pinfo;
	if (!kvs::read_text(root, stream, pinfo)) {
		TOGO_LOG_ERRORF(
			"failed to read KVS: [%2u,%2u]: %s\n",
			pinfo.line, pinfo.column, pinfo.message
		);
		return false;
	}
	return true;
}

/// Read text-format KVS from file.
bool kvs::read_text_file(KVS& root, StringRef const& path) {
	FileReader stream{};
	if (!stream.open(path)) {
		TOGO_LOG_ERRORF(
			"failed to read KVS from '%.*s': failed to open file\n",
			path.size, path.data
		);
		return false;
	}

	KVSParserInfo pinfo;
	bool const success = kvs::read_text(root, stream, pinfo);
	if (!success) {
		TOGO_LOG_ERRORF(
			"failed to read KVS from '%.*s': [%2u,%2u]: %s\n",
			path.size, path.data,
			pinfo.line, pinfo.column, pinfo.message
		);
	}
	stream.close();
	return success;
}

// write

namespace {

static constexpr char const TABS[]{
	"																"
};

#define RETURN_ERROR(x) if (!(x)) { return false; }

static bool write_tabs(IWriter& stream, unsigned tabs) {
	while (tabs > 0) {
		unsigned const amount = min(tabs, array_extent(TABS));
		if (!io::write(stream, TABS, amount)) {
			return false;
		}
		tabs -= amount;
	}
	return true;
}

TOGO_VALIDATE_FORMAT_PARAM(2, 3)
static bool writef(IWriter& stream, char const* const format, ...) {
	char buffer[256];
	va_list va;
	va_start(va, format);
	signed const size = std::vsnprintf(buffer, array_extent(buffer), format, va);
	va_end(va);
	if (size < 0) {
		return false;
	}
	return io::write(stream, buffer, static_cast<unsigned>(size));
}

static bool kvs_write(
	KVS const& kvs,
	IWriter& stream,
	KVSType const parent_type,
	unsigned const tabs,
	bool const is_root
);

static bool kvs_write_collection(
	KVS const& kvs,
	IWriter& stream,
	char const* const open,
	char const close,
	unsigned const tabs,
	bool const is_root
) {
	if (!is_root) {
		RETURN_ERROR(io::write(stream, open, kvs::empty(kvs) ? 1 : 2));
	}
	for (unsigned i = 0; i < kvs::size(kvs); ++i) {
		RETURN_ERROR(
			kvs_write(kvs[i], stream, kvs::type(kvs), tabs + !is_root, false) &&
			io::write_value(stream, '\n')
		);
	}
	if (!is_root) {
		RETURN_ERROR(
			(kvs::empty(kvs) || write_tabs(stream, tabs)) &&
			io::write_value(stream, close)
		);
	}
	return true;
}

inline static unsigned quote_level(StringRef const& str) {
	unsigned level = str.empty() ? 1 : 0;
	if (str.size >= 1) {
		if (is_number_lead(static_cast<signed>(str[0]))) {
			level = 1;
		}
	}
	auto const it_end = end(str);
	for (auto it = begin(str); it != it_end && level < 2; ++it) {
		switch (*it) {
		case '\t':
		case ' ':
		case ',': case ';':
		case '=':
		case '{': case '}':
		case '[': case ']':
		case '(': case ')':
		case '/':
		case '`':
			level = 1;
			break;

		case '\"': // fall-through
		case '\n':
			level = 2;
			break;
		}
	}
	return level;
}

inline static bool write_quote(IWriter& stream, unsigned level) {
	switch (level) {
	case 0: break;
	case 1: RETURN_ERROR(io::write_value(stream, '\"')); break;
	case 2: RETURN_ERROR(io::write(stream, "```", 3)); break;
	}
	return true;
}

inline static bool write_string(IWriter& stream, StringRef const& str) {
	unsigned const level = quote_level(str);
	RETURN_ERROR(
		write_quote(stream, level) &&
		io::write(stream, str.data, str.size) &&
		write_quote(stream, level)
	);
	return true;
}

static bool kvs_write(
	KVS const& kvs,
	IWriter& stream,
	KVSType const parent_type,
	unsigned const tabs,
	bool const is_root
) {
	RETURN_ERROR(write_tabs(stream, tabs));
	if (!is_root && parent_type == KVSType::node) {
		RETURN_ERROR(
			write_string(stream, kvs::name_ref(kvs)) &&
			io::write(stream, " = ", 3)
		);
	}
	switch (kvs::type(kvs)) {
	case KVSType::null:
		RETURN_ERROR(io::write(stream, "null", 4));
		break;

	case KVSType::integer:
		RETURN_ERROR(writef(stream, "%ld", kvs._value.integer));
		break;

	case KVSType::decimal:
		RETURN_ERROR(writef(stream, "%1.7lf", kvs._value.decimal));
		break;

	case KVSType::boolean:
		if (kvs._value.boolean) {
			RETURN_ERROR(io::write(stream, "true", 4));
		} else {
			RETURN_ERROR(io::write(stream, "false", 5));
		}
		break;

	case KVSType::string:
		RETURN_ERROR(write_string(stream, kvs::string_ref(kvs)));
		break;

	case KVSType::vec1:
		RETURN_ERROR(writef(
			stream, "(%1.7g)",
			kvs._value.vec1.x
		));
		break;

	case KVSType::vec2:
		RETURN_ERROR(writef(
			stream, "(%1.7g %1.7g)",
			kvs._value.vec2.x,
			kvs._value.vec2.y
		));
		break;

	case KVSType::vec3:
		RETURN_ERROR(writef(
			stream, "(%1.7g %1.7g %1.7g)",
			kvs._value.vec3.x,
			kvs._value.vec3.y,
			kvs._value.vec3.z
		));
		break;

	case KVSType::vec4:
		RETURN_ERROR(writef(
			stream, "(%1.7g %1.7g %1.7g %1.7g)",
			kvs._value.vec4.x,
			kvs._value.vec4.y,
			kvs._value.vec4.z,
			kvs._value.vec4.w
		));
		break;

	case KVSType::array:
		kvs_write_collection(kvs, stream, "[\n", ']', tabs, is_root);
		break;

	case KVSType::node:
		kvs_write_collection(kvs, stream, "{\n", '}', tabs, is_root);
		break;
	}
	return true;
}

#undef RETURN_ERROR

} // anonymous namespace

/// Write text-format KVS to stream.
///
/// Returns IO status of the stream (output is likely incomplete
/// if !status).
IOStatus kvs::write_text(KVS const& kvs, IWriter& stream) {
	kvs_write(kvs, stream, KVSType::node, 0, true);
	return io::status(stream);
}

/// Write text-format KVS to file.
bool kvs::write_text_file(KVS const& kvs, StringRef const& path) {
	FileWriter stream{};
	if (!stream.open(path, false)) {
		TOGO_LOG_ERRORF(
			"failed to write KVS to '%.*s': failed to open file\n",
			path.size, path.data
		);
		return false;
	}

	bool success = kvs::write_text(kvs, stream).ok();
	stream.close();
	return success;
}

} // namespace togo
