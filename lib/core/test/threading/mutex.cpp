
#include <togo/core/error/assert.hpp>
#include <togo/core/threading/mutex.hpp>

using namespace togo;

signed main() {
	Mutex m1{};
	mutex::lock(m1);
	TOGO_ASSERTE(!mutex::try_lock(m1));
	mutex::unlock(m1);

	{
		MutexLock m1_lock{m1};
		TOGO_ASSERTE(!mutex::try_lock(m1));
	}

	TOGO_ASSERTE(mutex::try_lock(m1));
	mutex::unlock(m1);

	return 0;
}
