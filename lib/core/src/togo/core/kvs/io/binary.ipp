#line 2 "togo/core/kvs/io/binary.ipp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/io/io.hpp>
#include <togo/core/kvs/kvs.hpp>
#include <togo/core/serialization/types.hpp>
#include <togo/core/serialization/serializer.hpp>
#include <togo/core/serialization/support.hpp>
#include <togo/core/serialization/binary_serializer.hpp>
#include <togo/core/serialization/array.hpp>
#include <togo/core/serialization/string.hpp>
#include <togo/core/serialization/vector.hpp>

namespace togo {
namespace {

static void kvs_read_binary(
	KVS& k_value,
	BinaryInputSerializer& ser,
	Array<char>& scratch
) {
	bool named_children = false;
	switch (kvs::type(k_value)) {
	case KVSType::null: break;
	case KVSType::integer: ser % k_value._value.integer; break;
	case KVSType::decimal: ser % k_value._value.decimal; break;
	case KVSType::boolean: ser % make_ser_proxy<u8>(k_value._value.boolean); break;
	case KVSType::string:
		ser % make_ser_collection<u32>(scratch);
		kvs::string(
			k_value,
			StringRef{
				array::begin(scratch),
				static_cast<unsigned>(array::size(scratch))
			}
		);
		break;

	case KVSType::vec1: ser % k_value._value.vec1; break;
	case KVSType::vec2: ser % k_value._value.vec2; break;
	case KVSType::vec3: ser % k_value._value.vec3; break;
	case KVSType::vec4: ser % k_value._value.vec4; break;

	case KVSType::node:
		named_children = true;
		// fall-through

	case KVSType::array: {
		u32 num_children;
		ser % num_children;
		kvs::resize(k_value, num_children);
		KVSType type;
		for (auto& k_child : k_value) {
			ser % type;
			kvs::set_type(k_child, type);
			if (named_children) {
				ser % make_ser_collection<u32>(scratch);
				kvs::set_name(
					k_child,
					StringRef{
						array::begin(scratch),
						static_cast<unsigned>(array::size(scratch))
					}
				);
			}
			kvs_read_binary(k_child, ser, scratch);
		}
	}	break;
	}
}

static void kvs_write_binary(
	KVS const& k_value,
	BinaryOutputSerializer& ser
) {
	StringRef string_ref;
	bool named_children = false;
	switch (kvs::type(k_value)) {
	case KVSType::null: break;
	case KVSType::integer: ser % k_value._value.integer; break;
	case KVSType::decimal: ser % k_value._value.decimal; break;
	case KVSType::boolean: ser % make_ser_proxy<u8>(k_value._value.boolean); break;
	case KVSType::string:
		string_ref = kvs::string_ref(k_value);
		ser % make_ser_string<u32>(string_ref);
		break;

	case KVSType::vec1: ser % k_value._value.vec1; break;
	case KVSType::vec2: ser % k_value._value.vec2; break;
	case KVSType::vec3: ser % k_value._value.vec3; break;
	case KVSType::vec4: ser % k_value._value.vec4; break;

	case KVSType::node:
		named_children = true;
		// fall-through

	case KVSType::array:
		ser % kvs::size(k_value);
		for (auto& k_child : k_value) {
			ser % kvs::type(k_child);
			if (named_children) {
				string_ref = kvs::name_ref(k_child);
				ser % make_ser_string<u32>(string_ref);
			}
			kvs_write_binary(k_child, ser);
		}
		break;
	}
}

} // anonymous namespace
} // namespace togo
