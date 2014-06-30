#line 2 "togo/log.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/log.hpp>
#include <togo/mutex.hpp>

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

// TODO: Print into buffer, then flush buffer to streams

namespace togo {

namespace {

static char const* const
_log_prefix[]{
	"",
	"debug: ",
	"error: ",
};

static Mutex _log_mutex{};

inline static void
vprintf(
	log::Type const l,
	char const* msg,
	va_list va
) {
	if (l != log::Type::general) {
		std::printf("%s", _log_prefix[static_cast<unsigned>(l)]);
	}
	std::vprintf(msg, va);
}

} // anonymous namespace

void
log::printf(
	log::Type const l,
	char const* msg,
	...
) {
	MutexLock log_mutex_lock{_log_mutex};
	va_list va;
	va_start(va, msg);
	togo::vprintf(l, msg, va);
	va_end(va);
}

void
log::printf_trace(
	log::Type const l,
	unsigned line,
	char const* file,
	char const* msg,
	...
) {
	MutexLock log_mutex_lock{_log_mutex};
	std::printf("%s @ %4d: ", file, line);
	va_list va;
	va_start(va, msg);
	togo::vprintf(l, msg, va);
	va_end(va);
}

} // namespace togo
