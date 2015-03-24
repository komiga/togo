#line 2 "togo/kvs/kvs.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/error/assert.hpp>
#include <togo/utility/utility.hpp>
#include <togo/log/log.hpp>
#include <togo/memory/memory.hpp>
#include <togo/string/string.hpp>
#include <togo/hash/hash.hpp>
#include <togo/kvs/kvs.hpp>

#include <cstring>

namespace togo {

namespace kvs {
	static void init_children(KVS& kvs, unsigned const from, unsigned const to);
	static void reset_children(KVS& kvs, unsigned const from, unsigned const to);
	static KVS const* find_impl(
		KVS const& kvs,
		StringRef const& name,
		KVSNameHash const name_hash
	);
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

KVS::KVS(KVS&& other)
	: _type(other._type)
	, _name_size(other._name_size)
	, _name_hash(other._name_hash)
	, _name(other._name)
	, _value(rvalue_ref(other._value))
{
	other._type = KVSType::null;
	other._name = nullptr;
	other._name_size = 0;
	other._name_hash = KVS_NAME_NULL;
}

KVS const* kvs::find_impl(
	KVS const& kvs,
	StringRef const& name,
	KVSNameHash const name_hash
) {
	if (name_hash == KVS_NAME_NULL || kvs::empty(kvs)) {
		return nullptr;
	}
	for (KVS const& item : kvs) {
		if (name_hash == kvs::name_hash(item)) {
			#if defined(TOGO_DEBUG)
			if (name.valid() && !string::compare_equal(name, kvs::name_ref(item))) {
				TOGO_LOG_DEBUGF(
					"hashes matched, but names mismatched: '%.*s' != '%.*s' (lookup_name != name)\n",
					name.size, name.data,
					kvs::name_size(item), kvs::name(item)
				);
			}
			#else
				(void)name;
			#endif
			return &item;
		}
	}
	return nullptr;
}

/// Find item in collection by name.
KVS* kvs::find(KVS& kvs, StringRef const& name) {
	KVSNameHash const name_hash = kvs::hash_name(name);
	return const_cast<KVS*>(kvs::find_impl(kvs, name, name_hash));
}

/// Find item in collection by name.
KVS const* kvs::find(KVS const& kvs, StringRef const& name) {
	KVSNameHash const name_hash = kvs::hash_name(name);
	return kvs::find_impl(kvs, name, name_hash);
}

/// Find item in collection by name hash.
KVS* kvs::find(KVS& kvs, KVSNameHash const name_hash) {
	return const_cast<KVS*>(
		kvs::find_impl(kvs, StringRef{}, name_hash)
	);
}

/// Find item in collection by name hash.
KVS const* kvs::find(KVS const& kvs, KVSNameHash const name_hash) {
	return kvs::find_impl(kvs, StringRef{}, name_hash);
}

/// Free value if dynamic and change type iff type differs.
///
/// Returns true if the type changed.
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

/// Set name.
void kvs::set_name(KVS& kvs, StringRef const& name) {
	if (name.empty()) {
		kvs::clear_name(kvs);
	} else if (name.size > kvs._name_size) {
		kvs::clear_name(kvs);
		kvs._name = static_cast<char*>(
			memory::default_allocator().allocate(name.size + 1, alignof(char))
		);
	}
	if (name.any()) {
		string::copy(kvs._name, name.size + 1, name);
	}
	kvs._name_size = name.size;
	kvs._name_hash = kvs::hash_name(name);
}

/// Clear name.
void kvs::clear_name(KVS& kvs) {
	if (kvs::is_named(kvs)) {
		TOGO_DESTROY(memory::default_allocator(), kvs._name);
		kvs._name = nullptr;
		kvs._name_size = 0;
		kvs._name_hash = KVS_NAME_NULL;
	}
}

/// Clear value.
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

/// Free dynamic value.
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

/// Copy value.
void kvs::copy(KVS& dst, KVS const& src) {
	kvs::set_name(dst, kvs::name_ref(src));
	switch (src._type) {
	case KVSType::integer: kvs::integer(dst, kvs::integer(src)); break;
	case KVSType::decimal: kvs::decimal(dst, kvs::decimal(src)); break;
	case KVSType::boolean: kvs::boolean(dst, kvs::boolean(src)); break;
	case KVSType::vec1: kvs::vec1(dst, kvs::vec1(src)); break;
	case KVSType::vec2: kvs::vec2(dst, kvs::vec2(src)); break;
	case KVSType::vec3: kvs::vec3(dst, kvs::vec3(src)); break;
	case KVSType::vec4: kvs::vec4(dst, kvs::vec4(src)); break;

	case KVSType::string:
		kvs::string(dst, kvs::string_ref(src));
		break;

	case KVSType::array: // fall-through
	case KVSType::node:
		kvs::set_type(dst, kvs::type(src));
		kvs::resize(dst, kvs::size(src));
		for (unsigned i = 0; i < kvs::size(dst); ++i) {
			kvs::copy(dst[i], src[i]);
		}
		break;

	case KVSType::null:
		kvs::nullify(dst);
		break;
	}
}

/// Move state.
///
/// src will be null and unnamed after this call.
void kvs::move(KVS& dst, KVS& src) {
	kvs::clear_name(dst);
	kvs::free_dynamic(dst);
	dst._type = src._type;
	dst._name = src._name;
	dst._name_size = src._name_size;
	dst._name_hash = src._name_hash;
	dst._value = rvalue_ref(src._value);

	src._type = KVSType::null;
	src._name = nullptr;
	src._name_size = 0;
	src._name_hash = KVS_NAME_NULL;
}

/// Set string value.
void kvs::string(KVS& kvs, StringRef const& value) {
	TOGO_ASSERTE(value.size == 0 || value.valid());
	kvs::set_type(kvs, KVSType::string);
	if (value.empty()) {
		kvs::free_dynamic(kvs);
	} else if (value.size >= kvs._value.string.capacity) {
		kvs::free_dynamic(kvs);
		kvs._value.string.capacity = value.size + 1;
		kvs._value.string.data = static_cast<char*>(
			memory::default_allocator().allocate(
				kvs._value.string.capacity,
				alignof(char)
			)
		);
	}
	if (value.any()) {
		string::copy(kvs._value.string.data, kvs._value.string.capacity, value);
	}
	kvs._value.string.size = value.size;
}

/// Change collection capacity.
///
/// If new_capacity is lower than the size, the collection is resized
/// to new_capacity.
void kvs::set_capacity(KVS& kvs, u32 const new_capacity) {
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
			memory::default_allocator().allocate(
				new_capacity * sizeof(KVS), alignof(KVS)
			)
		);
		if (kvs._value.collection.data) {
			std::memcpy(
				new_data,
				kvs._value.collection.data,
				kvs._value.collection.size * sizeof(KVS)
			);
		}
	}
	memory::default_allocator().deallocate(kvs._value.collection.data);
	kvs._value.collection.data = new_data;
	kvs._value.collection.capacity = new_capacity;
	kvs::init_children(kvs, kvs._value.collection.size, new_capacity);
}

/// Grow collection with a doubling factor.
///
/// Cost of insertion should be amortized O(1), assuming no aggressive
/// shrinking. Grows to at least min_capacity if it is non-zero.
void kvs::grow(KVS& kvs, u32 const min_capacity IGEN_DEFAULT(0))
{
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	u32 new_capacity = kvs._value.collection.capacity * 2 + 8;
	if (min_capacity > new_capacity) {
		new_capacity = min_capacity;
	}
	kvs::set_capacity(kvs, new_capacity);
}

/// Change collection size.
///
/// Upsize grows by using new_size as the minimum capacity.
/// If the collection grows, the new values will be null.
void kvs::resize(KVS& kvs, u32 const new_size) {
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

/// Remove an item by index.
void kvs::remove(KVS& kvs, unsigned const i) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	TOGO_ASSERTE(kvs::any(kvs));
	TOGO_ASSERTE(i < kvs._value.collection.size);
	u32 const new_size = kvs._value.collection.size - 1;
	if (i < new_size) {
		kvs._value.collection.data[i].~KVS();
		std::memmove(
			kvs._value.collection.data + i, kvs._value.collection.data + i + 1,
			(new_size - i) * sizeof(KVS)
		);
	}
	kvs::resize(kvs, new_size);
}

/// Remove an item by address.
///
/// If ptr is nullptr, an assertion will fail.
void kvs::remove(KVS& kvs, KVS const* const ptr) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	TOGO_ASSERTE(ptr != nullptr);
	TOGO_ASSERTE(kvs::any(kvs));
	TOGO_ASSERTE(kvs::begin(kvs) <= ptr && ptr < kvs::end(kvs));
	kvs::remove(kvs, (ptr - kvs._value.collection.data) / sizeof(KVS));
}

} // namespace togo
