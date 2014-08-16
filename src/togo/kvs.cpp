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

namespace kvs {
	static void init_children(KVS& kvs, unsigned const from, unsigned const to);
	static void reset_children(KVS& kvs, unsigned const from, unsigned const to);
} // namespace kvs

inline void kvs::init_children(KVS& kvs, unsigned const from, unsigned const to) {
	for (unsigned i = from; i < to; ++i) {
		new (&kvs._value.collection.data[i]) KVS();
	}
}

inline void kvs::reset_children(KVS& kvs, unsigned const from, unsigned const to) {
	for (unsigned i = from; i < to; ++i) {
		kvs._value.collection.data[i].~KVS();
	}
}

KVS::KVS(char const* const value, unsigned size)
	: KVS()
{
	kvs::string(*this, value, size);
}

bool kvs::set_type(KVS& kvs, KVSType const type) {
	if (kvs._type == type) {
		return false;
	}
	kvs::free_dynamic(kvs);
	kvs._type = type;

	// Ensure empty dynamic value when type changes
	if (kvs::is_type(kvs, KVSType::string)) {
		kvs._value.string.data = nullptr;
		kvs._value.string.size = 0;
		kvs._value.string.capacity = 0;
	} else if (kvs::is_type_any(kvs, type_mask_collection)) {
		kvs._value.collection.data = nullptr;
		kvs._value.collection.size = 0;
		kvs._value.collection.capacity = 0;
	}
	return true;
}

void kvs::set_name(KVS& kvs, char const* const name, unsigned size) {
	if (size > 0 && name[size - 1] == '\0') {
		--size;
	}
	if (size == 0) {
		kvs::clear_name(kvs);
	} else if (size > kvs._name_size) {
		kvs::clear_name(kvs);
		kvs._name = static_cast<char*>(
			memory::default_allocator().allocate(size + 1, alignof(char))
		);
	}
	if (size > 0) {
		std::memcpy(kvs._name, name, size);
		kvs._name[size] = '\0';
	}
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
		kvs._value.string.size = 0;
		break;

	case KVSType::array: // fall-through
	case KVSType::node:
		kvs::resize(kvs, 0);
		break;

	case KVSType::null:
		break;
	}
}

void kvs::free_dynamic(KVS& kvs) {
	if (kvs::is_type(kvs, KVSType::string)) {
		TOGO_DESTROY(memory::default_allocator(), kvs._value.string.data);
		kvs._value.string.data = nullptr;
		kvs._value.string.size = 0;
		kvs._value.string.capacity = 0;
	} else if (kvs::is_type_any(kvs, type_mask_collection)) {
		kvs::set_capacity(kvs, 0);
	}
}

void kvs::string(KVS& kvs, char const* const value, unsigned size) {
	TOGO_ASSERTE(size == 0 || value != nullptr);
	kvs::set_type(kvs, KVSType::string);
	if (size > 0 && value[size - 1] == '\0') {
		--size;
	}
	if (size == 0) {
		kvs::free_dynamic(kvs);
	} else if (size >= kvs._value.string.capacity) {
		kvs::free_dynamic(kvs);
		kvs._value.string.data = static_cast<char*>(
			memory::default_allocator().allocate(size + 1, alignof(char))
		);
		kvs._value.string.capacity = size + 1;
	}
	if (size > 0) {
		std::memcpy(kvs._value.string.data, value, size);
		kvs._value.string.data[size] = '\0';
	}
	kvs._value.string.size = size;
}

void kvs::set_capacity(KVS& kvs, unsigned const new_capacity) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	if (new_capacity == kvs._value.collection.capacity) {
		return;
	}

	if (new_capacity < kvs._value.collection.size) {
		kvs::reset_children(kvs, new_capacity, kvs._value.collection.size);
		kvs._value.collection.size = new_capacity;
	}

	KVS* new_data = nullptr;
	if (new_capacity != 0) {
		new_data = static_cast<KVS*>(
			memory::default_allocator().allocate(new_capacity * sizeof(KVS), alignof(KVS))
		);
		if (kvs._value.collection.data) {
			std::memcpy(new_data, kvs._value.collection.data, kvs._value.collection.size * sizeof(KVS));
		}
	}
	memory::default_allocator().deallocate(kvs._value.collection.data);
	kvs._value.collection.data = new_data;
	kvs._value.collection.capacity = new_capacity;
	kvs::init_children(kvs, kvs._value.collection.size, new_capacity);
}

void kvs::grow(KVS& kvs, unsigned const min_capacity) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	unsigned new_capacity = kvs._value.collection.capacity * 2 + 8;
	if (min_capacity > new_capacity) {
		new_capacity = min_capacity;
	}
	kvs::set_capacity(kvs, new_capacity);
}

void kvs::resize(KVS& kvs, unsigned const new_size) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	if (new_size > kvs._value.collection.capacity) {
		kvs::grow(kvs, new_size);
	} else if (new_size < kvs._value.collection.size) {
		kvs::reset_children(kvs, new_size, kvs._value.collection.size);
	} else if (new_size > kvs._value.collection.size) {
		kvs::reset_children(kvs, kvs._value.collection.size, new_size);
	}
	kvs._value.collection.size = new_size;
}

} // namespace togo
