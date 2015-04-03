#line 2 "togo/core/threading/mutex/posix.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/core/threading/condvar.hpp>

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

// condvar interface implementation

void condvar::signal(CondVar& cv, Mutex& /*m*/) {
	signed err = 0;
	POSIX_CHECK(pthread_cond_signal(&cv._impl.handle));
	return;

posix_error:
	TOGO_ASSERTF(false, "error signaling condvar: %d, %s", err, std::strerror(err));
}

void condvar::signal_all(CondVar& cv, Mutex& /*m*/) {
	signed err = 0;
	POSIX_CHECK(pthread_cond_broadcast(&cv._impl.handle));
	return;

posix_error:
	TOGO_ASSERTF(false, "error signaling condvar: %d, %s", err, std::strerror(err));
}

void condvar::wait(CondVar& cv, Mutex& m) {
	signed err = 0;
	POSIX_CHECK(pthread_cond_wait(&cv._impl.handle, &m._impl.handle));
	return;

posix_error:
	TOGO_ASSERTF(false, "error waiting on condvar: %d, %s", err, std::strerror(err));
}

} // namespace togo
