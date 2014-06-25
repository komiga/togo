#line 2 "togo/assert.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/assert.hpp>

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

namespace togo {

void
error_abort(
	unsigned line,
	char const* file,
	char const* msg,
	...
) {
	std::fprintf(stderr, "%s @ %4d: fatal error: ", file, line);
	va_list va;
	va_start(va, msg);
	std::vfprintf(stderr, msg, va);
	va_end(va);
	std::fflush(stderr);
	std::abort();
}

void
debug_print(
	unsigned line,
	char const* file,
	char const* msg,
	...
) {
	std::printf("%s @ %4d: debug: ", file, line);
	va_list va;
	va_start(va, msg);
	std::vprintf(msg, va);
	va_end(va);
}

} // namespace togo
