#line 2 "togo/utility/args.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/string/string.hpp>
#include <togo/kvs/kvs.hpp>
#include <togo/utility/args.hpp>

#include <utility>

namespace togo {

bool parse_args(
	KVS& k_options,
	KVS& k_command_options,
	KVS& k_command,
	signed const argc,
	char const* const argv[]
) {
	kvs::set_type(k_options, KVSType::node);
	kvs::set_type(k_command_options, KVSType::node);
	kvs::set_type(k_command, KVSType::node);
	kvs::clear(k_options);
	kvs::clear(k_command_options);
	kvs::clear(k_command);
	if (argc == 0) {
		return false;
	}
	kvs::set_name(k_options, StringRef{argv[0], cstr_tag{}});

	signed aidx;
	char const* str;
	signed pos;
	signed pos_eq;
	bool dashed;
	KVS* k_current = &k_options;
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
		if (pos_eq == 0) {
			pos_eq = pos;
		}
		if (dashed) {
			KVS& back = kvs::push_back(
				*k_current,
				KVS{StringRef{str, unsigned_cast(pos_eq)}, null_tag{}}
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
		} else if (k_current == &k_options) {
			// Command reached; switch to parse leading options
			k_current = &k_command_options;
			kvs::set_name(k_command, StringRef{str, unsigned_cast(pos)});
		} else {
			// Undashed argument to command
			k_current = &k_command;
			kvs::push_back(*k_current, KVS{StringRef{str, unsigned_cast(pos)}});
		}
	}
	return k_current != &k_options;
}

} // namespace togo
