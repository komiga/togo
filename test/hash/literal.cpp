
#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/hash.hpp>

#include <cstdio>

using namespace togo;

namespace {

using namespace togo::hash;

static constexpr hash32 const
h32_identity = ""_hash32,
h32_test = "test"_hash32;

static constexpr hash64 const
h64_identity = ""_hash64,
h64_test = "test"_hash64;

} // anonymous namespace

signed
main() {
	TOGO_ASSERTE(hash::IDENTITY32 == h32_identity);
	TOGO_ASSERTE(hash::calc32("test", 4) == h32_test);
	std::printf(
		"32: identity = 0x%08x, \"test\" = 0x%08x\n",
		h32_identity, h32_test
	);

	TOGO_ASSERTE(hash::IDENTITY64 == h64_identity);
	TOGO_ASSERTE(hash::calc64("test", 4) == h64_test);
	std::printf(
		"64: identity = 0x%016lx, \"test\" = 0x%016lx\n",
		h64_identity, h64_test
	);
	return 0;
}
