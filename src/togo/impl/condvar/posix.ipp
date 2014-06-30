#line 2 "togo/impl/mutex/posix.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/threading_types.hpp>
#include <togo/memory_types.hpp>
#include <togo/assert.hpp>
#include <togo/mutex.hpp>
#include <togo/condvar.hpp>

#include <cerrno>
#include <cstring>

#include <pthread.h>

namespace togo {

#define POSIX_CHECK(f) \
	if (((err) = (f))) { goto posix_error; }

PosixCondVarImpl::~PosixCondVarImpl() {
	if (pthread_cond_destroy(&handle) == EBUSY) {
		TOGO_ASSERT(false, "cannot destroy in-use condvar");
	}
}

// mutex interface implementation

void condvar::signal(CondVar& cv, MutexLock& /*l*/) {
	signed err = 0;
	POSIX_CHECK(pthread_cond_signal(&cv._impl.handle));
	return;

posix_error:
	TOGO_ASSERTF(false, "error signaling condvar: %d, %s", err, std::strerror(err));
}

void condvar::wait(CondVar& cv, MutexLock& l) {
	signed err = 0;
	POSIX_CHECK(pthread_cond_wait(&cv._impl.handle, &l._mutex._impl.handle));
	return;

posix_error:
	TOGO_ASSERTF(false, "error waiting on condvar: %d, %s", err, std::strerror(err));
}

} // namespace togo
