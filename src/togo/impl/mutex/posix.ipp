#line 2 "togo/impl/mutex/posix.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/threading_types.hpp>
#include <togo/assert.hpp>
#include <togo/mutex.hpp>

#include <cerrno>
#include <cstring>

#include <pthread.h>

namespace togo {

#define POSIX_CHECK(f) \
	if (((err) = (f))) { goto posix_error; }

PosixMutexImpl::PosixMutexImpl(Mutex& m)
	: handle(
		#if defined(TOGO_DEBUG)
			PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
		#else
			PTHREAD_MUTEX_INITIALIZER
		#endif
	)
{
	if (m._type == MutexType::recursive) {
		handle = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	}
}

PosixMutexImpl::~PosixMutexImpl() {
	if (pthread_mutex_destroy(&handle) == EBUSY) {
		TOGO_ASSERT(false, "cannot destroy locked mutex");
	}
}

// mutex interface implementation

bool mutex::try_lock(Mutex& m) {
	signed err = pthread_mutex_trylock(&m._impl.handle);
	if (err == EBUSY) {
		return false;
	} else if (err) {
		goto posix_error;
	}
	return true;

posix_error:
	TOGO_ASSERTF(false, "error try-locking mutex: %d, %s", err, std::strerror(err));
}

void mutex::lock(Mutex& m) {
	signed err = 0;
	POSIX_CHECK(pthread_mutex_lock(&m._impl.handle));
	return;

posix_error:
	TOGO_ASSERTF(false, "error locking mutex: %d, %s", err, std::strerror(err));
}

void mutex::unlock(Mutex& m) {
	signed err = 0;
	POSIX_CHECK(pthread_mutex_unlock(&m._impl.handle));
	return;

posix_error:
	TOGO_ASSERTF(false, "error unlocking mutex: %d, %s", err, std::strerror(err));
}

} // namespace togo
