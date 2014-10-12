#line 2 "togo/args.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/string.hpp>
#include <togo/kvs.hpp>
#include <togo/args.hpp>

#include <utility>

namespace togo {

bool parse_args(
	KVS& options,
	KVS& command,
	signed const argc,
	char const* const argv[]
) {
	kvs::set_type(options, KVSType::node);
	kvs::set_type(command, KVSType::node);
	kvs::clear(options);
	kvs::clear(command);
	if (argc == 0) {
		return false;
	}
	kvs::set_name(options, StringRef{argv[0], cstr_tag{}});

	signed aidx;
	char const* str;
	signed pos;
	signed pos_eq;
	bool dashed;
	KVS* current = &options;
	for (aidx = 1; argc > aidx; ++aidx) {
		str = argv[aidx];
		pos = 0;
		pos_eq = 0;
		dashed = false;
		for (; str[pos] != '\0'; ++pos) {
			if (pos == 0 && str[pos] == '-') {
				dashed = true;
			} else if (str[pos] == '=') {
				pos_eq = pos;
				break;
			}
		}
		for (; str[pos] != '\0'; ++pos) {
			// Continue to the end of the string
			// (pos becomes size of string)
		}
		if (dashed) {
			if (pos_eq == 0) {
				pos_eq = pos;
			}
			KVS& back = kvs::push_back(
				*current,
				{StringRef{str, unsigned_cast(pos_eq)}, null_tag{}}
			);
			if (pos > pos_eq) {
				++pos_eq;
				// TODO: Parse string to typed value
				kvs::string(
					back,
					StringRef{str + pos_eq, unsigned_cast(pos - pos_eq)}
				);
			} else {
				kvs::boolean(back, true);
			}
		} else if (current != &command) {
			// Command reached
			current = &command;
			kvs::set_name(*current, {StringRef{str, unsigned_cast(pos)}});
		} else {
			// Undashed argument to command
			kvs::push_back(*current, {StringRef{str, unsigned_cast(pos)}});
		}
	}
	return current == &command;
}

} // namespace togo
