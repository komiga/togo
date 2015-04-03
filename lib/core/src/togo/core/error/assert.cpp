#line 2 "togo/core/error/assert.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/error/assert.hpp>

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

namespace togo {

void error_abort(unsigned line, char const* file, char const* msg, ...) {
	std::fprintf(stderr, "%s @ %4d: fatal error: ", file, line);
	va_list va;
	va_start(va, msg);
	std::vfprintf(stderr, msg, va);
	va_end(va);
	std::fflush(stderr);
	std::abort();
}

} // namespace togo
