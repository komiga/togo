
#include <togo/assert.hpp>
#include <togo/mutex.hpp>

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
