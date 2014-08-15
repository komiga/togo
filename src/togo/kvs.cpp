#line 2 "togo/kvs.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/assert.hpp>
#include <togo/hash.hpp>
#include <togo/memory.hpp>
#include <togo/kvs.hpp>

#include <cstring>

namespace togo {

namespace {
	static constexpr KVSType const type_mask_collection
		= KVSType::array
		| KVSType::node
	;
} // anonymous namespace

KVS::KVS(char const* const value, unsigned const size)
	: KVS()
{
	kvs::string(*this, value, size);
}

bool kvs::set_type(KVS& kvs, KVSType const type) {
	if (kvs._type == type) {
		return false;
	}
	/// Only clear value if dynamic
	if (kvs::is_type_any(kvs, type_mask_collection | KVSType::string)) {
		kvs::clear(kvs);
	}
	kvs._type = type;
	if (kvs::is_type(kvs, KVSType::string)) {
		kvs._value.string.data = nullptr;
		kvs._value.string.size = 0;
	} else if (kvs::is_type_any(kvs, type_mask_collection)) {
		kvs._value.collection.data = nullptr;
		kvs._value.collection.size = 0;
	}
	return true;
}

void kvs::set_name(KVS& kvs, char const* const name, unsigned const size) {
	if (size > kvs._name_size) {
		TOGO_DESTROY(memory::default_allocator(), kvs._name);
		kvs._name = static_cast<char*>(memory::default_allocator().allocate(size, alignof(char)));
	}
	std::memcpy(kvs._name, name, size);
	kvs._name_size = size;
	kvs._name_hash = hash::calc64(name, size);
}

void kvs::clear_name(KVS& kvs) {
	if (kvs::is_named(kvs)) {
		TOGO_DESTROY(memory::default_allocator(), kvs._name);
		kvs._name = nullptr;
		kvs._name_size = 0;
		kvs._name_hash = hash::IDENTITY64;
	}
}

void kvs::clear(KVS& kvs) {
	switch (kvs._type) {
	case KVSType::integer: kvs._value.integer = 0; break;
	case KVSType::decimal: kvs._value.decimal = 0.0f; break;
	case KVSType::boolean: kvs._value.boolean = false; break;
	case KVSType::vec1: kvs._value.vec1 = Vec1{}; break;
	case KVSType::vec2: kvs._value.vec2 = Vec2{}; break;
	case KVSType::vec3: kvs._value.vec3 = Vec3{}; break;
	case KVSType::vec4: kvs._value.vec4 = Vec4{}; break;

	case KVSType::string:
		if (kvs._value.string.data) {
			TOGO_DESTROY(memory::default_allocator(), kvs._value.string.data);
			kvs._value.string.data = nullptr;
			kvs._value.string.size = 0;
		}
		break;

	case KVSType::array: // fall-through
	case KVSType::node:
		if (kvs._value.collection.data) {
			for (auto& sub_kvs : kvs) {
				kvs::clear(sub_kvs);
			}
			TOGO_DESTROY(memory::default_allocator(), kvs._value.collection.data);
			kvs._value.collection.data = nullptr;
			kvs._value.collection.size = 0;
		}
		break;

	case KVSType::null:
		break;
	}
}

void kvs::string(KVS& kvs, char const* const value, unsigned const size) {
	if (!kvs::is_type(kvs, KVSType::string) || size > kvs._value.string.size) {
		if (!kvs::set_type(kvs, KVSType::string)) {
			kvs::clear(kvs);
		}
		kvs._value.string.data = static_cast<char*>(memory::default_allocator().allocate(size, alignof(char)));
	}
	kvs._value.string.size = size;
	std::memcpy(kvs._value.string.data, value, size);
}

} // namespace togo
