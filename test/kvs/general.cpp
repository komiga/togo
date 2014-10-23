
#include <togo/utility.hpp>
#include <togo/string.hpp>
#include <togo/assert.hpp>
#include <togo/log.hpp>
#include <togo/kvs.hpp>

#include "../common/helpers.hpp"

#include <cstring>

using namespace togo;

signed main() {
	memory_init();

	TOGO_LOGF("sizeof(KVS) = %u\n", static_cast<unsigned>(sizeof(KVS)));
	TOGO_LOGF("sizeof(KVS::Value) = %u\n", static_cast<unsigned>(sizeof(KVS::Value)));
	TOGO_LOGF("sizeof(KVS::StringValue) = %u\n", static_cast<unsigned>(sizeof(KVS::StringValue)));
	TOGO_LOGF("sizeof(KVS::CollectionValue) = %u\n", static_cast<unsigned>(sizeof(KVS::CollectionValue)));
	TOGO_LOGF("sizeof(Vec4) = %u\n", static_cast<unsigned>(sizeof(Vec4)));

	TOGO_LOGF("alignof(KVS) %u\n", static_cast<unsigned>(alignof(KVS)));
	TOGO_LOGF("alignof(KVS::Value) %u\n", static_cast<unsigned>(alignof(KVS::Value)));

	{
		KVS const identity;
		TOGO_ASSERTE(kvs::is_null(identity));
		TOGO_ASSERTE(!kvs::is_integer(identity));
		TOGO_ASSERTE(!kvs::is_decimal(identity));
		TOGO_ASSERTE(!kvs::is_boolean(identity));
		TOGO_ASSERTE(!kvs::is_string(identity));
		TOGO_ASSERTE(!kvs::is_vec1(identity));
		TOGO_ASSERTE(!kvs::is_vec2(identity));
		TOGO_ASSERTE(!kvs::is_vec3(identity));
		TOGO_ASSERTE(!kvs::is_vec4(identity));
		TOGO_ASSERTE(!kvs::is_vector(identity));
		TOGO_ASSERTE(!kvs::is_node(identity));
		TOGO_ASSERTE(!kvs::is_array(identity));
		TOGO_ASSERTE(!kvs::is_collection(identity));
		TOGO_ASSERTE(!kvs::is_named(identity));
		TOGO_ASSERTE(kvs::size(identity) == 0);
		TOGO_ASSERTE(!kvs::any(identity));
		TOGO_ASSERTE(kvs::empty(identity));
		TOGO_ASSERTE(kvs::begin(identity) == nullptr);
		TOGO_ASSERTE(kvs::end(identity) == nullptr);
		TOGO_ASSERTE(kvs::find(identity, KVS_NAME_NULL) == nullptr);
		TOGO_ASSERTE(kvs::find(identity, "") == nullptr);
	}

	{
		static constexpr char const NAME[]{"bzork"};
		static constexpr char const VALUE[]{"fitz"};
		KVS a{VALUE};
		TOGO_ASSERTE(!kvs::is_named(a));
		TOGO_ASSERTE(kvs::is_string(a));
		TOGO_ASSERTE(kvs::string_size(a) == string::size(VALUE));
		TOGO_ASSERTE(string::compare_equal(VALUE, kvs::string_ref(a)));

		kvs::set_name(a, NAME);
		TOGO_ASSERTE(kvs::is_named(a));
		TOGO_ASSERTE(kvs::name_size(a) == string::size(NAME));
		TOGO_ASSERTE(string::compare_equal(NAME, kvs::name_ref(a)));

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

	{
		KVS x{"x-name", "x-value"};

		KVS y;
		kvs::copy(y, x);
		TOGO_ASSERTE(kvs::is_type(y, kvs::type(x)));
		TOGO_ASSERTE(kvs::name_size(y) == kvs::name_size(x));
		TOGO_ASSERTE(string::compare_equal(kvs::name_ref(y), kvs::name_ref(x)));
		TOGO_ASSERTE(kvs::string_size(y) == kvs::string_size(x));
		TOGO_ASSERTE(string::compare_equal(kvs::string_ref(y), kvs::string_ref(x)));

		KVS z{"z-name", "z-value"};
		kvs::move(z, x);
		TOGO_ASSERTE(kvs::is_null(x));
		TOGO_ASSERTE(!kvs::is_type(z, kvs::type(x)));
		TOGO_ASSERTE(kvs::name_size(z) == string::size("x-name"));
		TOGO_ASSERTE(string::compare_equal(kvs::name_ref(z), "x-name"));
		TOGO_ASSERTE(kvs::string_size(z) == string::size("x-value"));
		TOGO_ASSERTE(string::compare_equal(kvs::string_ref(z), "x-value"));
	}

	{
		KVS c;
		kvs::set_type(c, KVSType::array);
		TOGO_ASSERTE(kvs::is_array(c));
		TOGO_ASSERTE(kvs::is_collection(c));
		TOGO_ASSERTE(kvs::size(c) == 0);
		TOGO_ASSERTE(kvs::capacity(c) == 0);
		TOGO_ASSERTE(kvs::space(c) == 0);
		TOGO_ASSERTE(!kvs::any(c));
		TOGO_ASSERTE(kvs::empty(c));
		kvs::resize(c, 1);
		TOGO_ASSERTE(kvs::size(c) == 1);
		TOGO_ASSERTE(kvs::capacity(c) == 8);
		TOGO_ASSERTE(kvs::space(c) == 7);
		TOGO_ASSERTE(kvs::any(c));
		TOGO_ASSERTE(!kvs::empty(c));
		TOGO_ASSERTE(kvs::is_null(kvs::back(c)));
		TOGO_ASSERTE(kvs::find(c, KVS_NAME_NULL) == nullptr);
		TOGO_ASSERTE(kvs::find(c, "") == nullptr);
		TOGO_ASSERTE(kvs::find(c, "x") == nullptr);
	}

	{
		KVS c{"c", KVSType::node};
		TOGO_ASSERTE(kvs::is_node(c));
		TOGO_ASSERTE(kvs::is_collection(c));
		TOGO_ASSERTE(kvs::is_named(c));
		TOGO_ASSERTE(string::compare_equal("c", kvs::name_ref(c)));
		TOGO_ASSERTE(kvs::size(c) == 0);
		TOGO_ASSERTE(kvs::capacity(c) == 0);
		TOGO_ASSERTE(kvs::space(c) == 0);
		TOGO_ASSERTE(!kvs::any(c));
		TOGO_ASSERTE(kvs::empty(c));

		{
			kvs::resize(c, 2);
			KVS& item0 = c[0];
			kvs::set_name(item0, "item0");
			KVS& item2 = kvs::push_back(c, {"item2", null_tag{}});
			TOGO_ASSERTE(kvs::size(c) == 3);
			TOGO_ASSERTE(kvs::find(c, "item0") == &item0);
			TOGO_ASSERTE(kvs::find(c, "item1") == nullptr);
			TOGO_ASSERTE(kvs::find(c, "item2") == &item2);
		}

		kvs::pop_back(c);
		TOGO_ASSERTE(kvs::size(c) == 2);
		kvs::remove(c, unsigned{0});
		TOGO_ASSERTE(kvs::size(c) == 1);
		kvs::pop_back(c);
		TOGO_ASSERTE(kvs::empty(c));
	}

	{
		KVS v{};
		// kvs::find() short-circuits when KVS is non-collection
		TOGO_ASSERTE(kvs::find(v, "x") == nullptr);
	}

	{
		KVS null{};
		KVS integer{1};
		KVS decimal{1.0};
		KVS boolean{true, bool_tag{}};
		KVS string{"value"};
		KVS vec1{Vec1{1}};
		KVS vec2{Vec2{1, 2}};
		KVS vec3{Vec3{1, 2, 3}};
		KVS vec4{Vec4{1, 2, 3, 4}};

		KVS null_n{"null", null_tag{}};
		KVS integer_n{"integer", s64{1}};
		KVS decimal_n{"decimal", 1.0};
		KVS boolean_n{"boolean", true, bool_tag{}};
		KVS string_n{"string", "value"};
		KVS vec1_n{"vec1", Vec1{1}};
		KVS vec2_n{"vec2", Vec2{1, 2}};
		KVS vec3_n{"vec3", Vec3{1, 2, 3}};
		KVS vec4_n{"vec4", Vec4{1, 2, 3, 4}};

		TOGO_ASSERTE(!kvs::is_named(null) && kvs::is_null(null));
		TOGO_ASSERTE(!kvs::is_named(integer) && kvs::is_integer(integer));
		TOGO_ASSERTE(!kvs::is_named(decimal) && kvs::is_decimal(decimal));
		TOGO_ASSERTE(!kvs::is_named(boolean) && kvs::is_boolean(boolean));
		TOGO_ASSERTE(!kvs::is_named(string) && kvs::is_string(string));
		TOGO_ASSERTE(!kvs::is_named(vec1) && kvs::is_vec1(vec1));
		TOGO_ASSERTE(!kvs::is_named(vec2) && kvs::is_vec2(vec2));
		TOGO_ASSERTE(!kvs::is_named(vec3) && kvs::is_vec3(vec3));
		TOGO_ASSERTE(!kvs::is_named(vec4) && kvs::is_vec4(vec4));

		TOGO_ASSERTE(kvs::is_named(null_n) && kvs::is_null(null_n));
		TOGO_ASSERTE(kvs::is_named(integer_n) && kvs::is_integer(integer_n));
		TOGO_ASSERTE(kvs::is_named(decimal_n) && kvs::is_decimal(decimal_n));
		TOGO_ASSERTE(kvs::is_named(boolean_n) && kvs::is_boolean(boolean_n));
		TOGO_ASSERTE(kvs::is_named(string_n) && kvs::is_string(string_n));
		TOGO_ASSERTE(kvs::is_named(vec1_n) && kvs::is_vec1(vec1_n));
		TOGO_ASSERTE(kvs::is_named(vec2_n) && kvs::is_vec2(vec2_n));
		TOGO_ASSERTE(kvs::is_named(vec3_n) && kvs::is_vec3(vec3_n));
		TOGO_ASSERTE(kvs::is_named(vec4_n) && kvs::is_vec4(vec4_n));
	}

	return 0;
}
