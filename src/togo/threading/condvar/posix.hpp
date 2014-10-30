#line 2 "togo/threading/condvar/posix.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/config.hpp>

#include <pthread.h>

namespace togo {

// Forward declarations
struct CondVar;

struct PosixCondVarImpl {
	pthread_cond_t handle;

	PosixCondVarImpl(CondVar&)
		: handle(PTHREAD_COND_INITIALIZER)
	{}
	~PosixCondVarImpl();
};

using CondVarImpl = PosixCondVarImpl;

} // namespace togo
