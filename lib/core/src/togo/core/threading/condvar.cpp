#line 2 "togo/core/threading/condvar.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/threading/types.hpp>
#include <togo/core/threading/condvar.hpp>

#if defined(TOGO_PLATFORM_IS_POSIX)
	#include <togo/core/threading/condvar/posix.ipp>
#else
	#error "missing CondVar implementation for target platform"
#endif
