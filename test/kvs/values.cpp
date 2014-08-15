
#include <togo/utility.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/kvs.hpp>

#include "../common/helpers.hpp"

#include <cstring>

using namespace togo;

signed
main() {
	memory_init();

	TOGO_LOGF("sizeof(KVS) = %u\n", static_cast<unsigned>(sizeof(KVS)));
	TOGO_LOGF("sizeof(KVS::Value) = %u\n", static_cast<unsigned>(sizeof(KVS::Value)));
	TOGO_LOGF("alignof(KVS) %u\n", static_cast<unsigned>(alignof(KVS)));
	TOGO_LOGF("alignof(KVS::Value) %u\n", static_cast<unsigned>(alignof(KVS::Value)));

	{
		KVS const identity;
		TOGO_ASSERTE(kvs::is_null(identity));
		TOGO_ASSERTE(!kvs::is_integer(identity));
		TOGO_ASSERTE(!kvs::is_decimal(identity));
		TOGO_ASSERTE(!kvs::is_boolean(identity));
		TOGO_ASSERTE(!kvs::is_string(identity));
		TOGO_ASSERTE(!kvs::is_vector(identity));
		TOGO_ASSERTE(!kvs::is_collection(identity));
		TOGO_ASSERTE(!kvs::is_named(identity));
		TOGO_ASSERTE(kvs::size(identity) == 0);
		TOGO_ASSERTE(!kvs::any(identity));
		TOGO_ASSERTE(kvs::empty(identity));
		TOGO_ASSERTE(kvs::begin(identity) == nullptr);
		TOGO_ASSERTE(kvs::end(identity) == nullptr);
	}

	{
		static constexpr char const NAME[]{"bzork"};
		static constexpr char const VALUE[]{"fitz"};
		KVS a{VALUE};
		TOGO_ASSERTE(!kvs::is_named(a));
		TOGO_ASSERTE(kvs::is_string(a));
		TOGO_ASSERTE(kvs::string_size(a) == sizeof(VALUE));
		TOGO_ASSERTE(0 == std::strncmp(VALUE, kvs::string(a), kvs::string_size(a)));

		kvs::set_name(a, NAME);
		TOGO_ASSERTE(kvs::is_named(a));
		TOGO_ASSERTE(kvs::name_size(a) == sizeof(NAME));
		TOGO_ASSERTE(0 == std::strncmp(NAME, kvs::name(a), kvs::name_size(a)));

		TOGO_LOGF("\"%s\" = \"%s\"\n", kvs::name(a), kvs::string(a));

		kvs::integer(a, static_cast<s64>(0xCECECECECECECECE));
		TOGO_ASSERTE(kvs::is_integer(a));
		TOGO_ASSERTE(kvs::integer(a) == static_cast<s64>(0xCECECECECECECECE));

		kvs::boolean(a, false);
		TOGO_ASSERTE(kvs::is_boolean(a));
		TOGO_ASSERTE(kvs::boolean(a) == false);

		kvs::nullify(a);
		TOGO_ASSERTE(kvs::is_null(a));
	}

	return 0;
}
