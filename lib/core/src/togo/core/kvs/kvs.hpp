#line 2 "togo/core/kvs/kvs.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief KVS interface.
@ingroup lib_core_kvs
*/

#pragma once

// igen-source: kvs/io_text.cpp
// igen-source: kvs/io_binary.cpp

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/error/assert.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/string/string.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/io/types.hpp>
#include <togo/core/kvs/types.hpp>
#include <togo/core/kvs/kvs.gen_interface>

namespace togo {

namespace {
	static constexpr KVSType const type_mask_vector
		= KVSType::vec1
		| KVSType::vec2
		| KVSType::vec3
		| KVSType::vec4
	;
	/*static constexpr KVSType const type_mask_value
		= KVSType::integer
		| KVSType::decimal
		| KVSType::boolean
		| KVSType::string
		| type_mask_vector
	;
	static constexpr KVSType const type_mask_value_nullable
		= KVSType::null
		| type_mask_value
	;*/
	static constexpr KVSType const type_mask_collection
		= KVSType::array
		| KVSType::node
	;
} // anonymous namespace

namespace kvs {

/**
	@addtogroup lib_core_kvs
	@{
*/

/// Calculate KVS name hash of string.
inline KVSNameHash hash_name(StringRef const& name) {
	return hash::calc32(name);
}

/// Type.
inline KVSType type(KVS const& kvs) {
	return kvs._type;
}

/// Whether value type is type.
inline bool is_type(KVS const& kvs, KVSType const type) {
	return kvs._type == type;
}

/// Whether value type is any in type.
inline bool is_type_any(KVS const& kvs, KVSType const type) {
	return enum_bool(kvs._type & type);
}

/// Whether type is KVSType::null.
inline bool is_null(KVS const& kvs) { return kvs::is_type(kvs, KVSType::null); }

/// Whether type is KVSType::integer.
inline bool is_integer(KVS const& kvs) { return kvs::is_type(kvs, KVSType::integer); }

/// Whether type is KVSType::decimal.
inline bool is_decimal(KVS const& kvs) { return kvs::is_type(kvs, KVSType::decimal); }

/// Whether type is KVSType::boolean.
inline bool is_boolean(KVS const& kvs) { return kvs::is_type(kvs, KVSType::boolean); }

/// Whether type is KVSType::string.
inline bool is_string(KVS const& kvs) { return kvs::is_type(kvs, KVSType::string); }

/// Whether type is KVSType::vec1.
inline bool is_vec1(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec1); }

/// Whether type is KVSType::vec2.
inline bool is_vec2(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec2); }

/// Whether type is KVSType::vec3.
inline bool is_vec3(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec3); }

/// Whether type is KVSType::vec4.
inline bool is_vec4(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec4); }

/// Whether type is KVSType::node.
inline bool is_node(KVS const& kvs) { return kvs::is_type(kvs, KVSType::node); }

/// Whether type is KVSType::array.
inline bool is_array(KVS const& kvs) { return kvs::is_type(kvs, KVSType::array); }

/// Whether type is a vector type.
inline bool is_vector(KVS const& kvs) { return kvs::is_type_any(kvs, type_mask_vector); }

/// Whether type is an array or node.
inline bool is_collection(KVS const& kvs) { return kvs::is_type_any(kvs, type_mask_collection); }

/// Name (NUL-terminated).
inline char const* name(KVS const& kvs) {
	return kvs._name ? kvs._name : "";
}

/// Name size (not including NUL terminator).
inline u32 name_size(KVS const& kvs) {
	return kvs._name_size;
}

/// Name reference.
inline StringRef name_ref(KVS const& kvs) {
	return StringRef{kvs._name, kvs._name_size};
}

/// Name hash.
inline KVSNameHash name_hash(KVS const& kvs) {
	return kvs._name_hash;
}

/// Whether name is non-empty.
inline bool is_named(KVS const& kvs) {
	return kvs._name;
}

/// Number of values in the collection.
inline u32 size(KVS const& kvs) {
	return kvs::is_collection(kvs) ? kvs._value.collection.size : 0;
}

/// Number of values reserved in the collection.
inline u32 capacity(KVS const& kvs) {
	return kvs::is_collection(kvs) ? kvs._value.collection.capacity : 0;
}

/// Number of values that can be added before a resize occurs.
inline u32 space(KVS const& kvs) {
	return
		kvs::is_collection(kvs)
		? kvs._value.collection.capacity - kvs._value.collection.size
		: 0
	;
}

/// Whether there are any values in the collection.
inline bool any(KVS const& kvs) {
	return kvs::is_collection(kvs) ? kvs._value.collection.size : false;
}

/// Whether there are no values in the collection.
inline bool empty(KVS const& kvs) {
	return kvs::is_collection(kvs) ? kvs._value.collection.size == 0 : true;
}

/// Beginning iterator: [begin, end).
inline KVS* begin(KVS& kvs) {
	return
		kvs::is_collection(kvs)
		? kvs._value.collection.data
		: nullptr
	;
}
/// Beginning iterator: [begin, end).
inline KVS const* begin(KVS const& kvs) {
	return
		kvs::is_collection(kvs)
		? kvs._value.collection.data
		: nullptr
	;
}

/// Ending iterator: [begin, end).
inline KVS* end(KVS& kvs) {
	return
		kvs::is_collection(kvs)
		? kvs._value.collection.data + kvs._value.collection.size
		: nullptr
	;
}
/// Ending iterator: [begin, end).
inline KVS const* end(KVS const& kvs) {
	return
		kvs::is_collection(kvs)
		? kvs._value.collection.data + kvs._value.collection.size
		: nullptr
	;
}

/// Integer value.
inline s64 integer(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::integer));
	return kvs._value.integer;
}

/// Decimal value.
inline f64 decimal(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::decimal));
	return kvs._value.decimal;
}

/// Boolean value.
inline bool boolean(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::boolean));
	return kvs._value.boolean;
}

/// String value (NUL-terminated).
inline char const* string(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return kvs._value.string.data ? kvs._value.string.data : "";
}

/// String value size (not including NUL terminator).
inline u32 string_size(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return kvs._value.string.size;
}

/// String value reference.
inline StringRef string_ref(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return StringRef{
		kvs._value.string.data ? kvs._value.string.data : "",
		kvs._value.string.size
	};
}

/// 1-dimensional vector value.
inline Vec1 const& vec1(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec1));
	return kvs._value.vec1;
}

/// 2-dimensional vector value.
inline Vec2 const& vec2(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec2));
	return kvs._value.vec2;
}

/// 3-dimensional vector value.
inline Vec3 const& vec3(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec3));
	return kvs._value.vec3;
}

/// 4-dimensional vector value.
inline Vec4 const& vec4(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec4));
	return kvs._value.vec4;
}

/// Free value if dynamic and change type to KVSType::null.
inline void nullify(KVS& kvs) {
	kvs::set_type(kvs, KVSType::null);
}

/// Set integer value.
inline void integer(KVS& kvs, s64 const value) {
	kvs::set_type(kvs, KVSType::integer);
	kvs._value.integer = value;
}

/// Set decimal value.
inline void decimal(KVS& kvs, f64 const value) {
	kvs::set_type(kvs, KVSType::decimal);
	kvs._value.decimal = value;
}

/// Set boolean value.
inline void boolean(KVS& kvs, bool const value) {
	kvs::set_type(kvs, KVSType::boolean);
	kvs._value.boolean = value;
}

/// Set 1-dimensional vector value.
inline void vec1(KVS& kvs, Vec1 const& value) {
	kvs::set_type(kvs, KVSType::vec1);
	kvs._value.vec1 = value;
}

/// Set 2-dimensional vector value.
inline void vec2(KVS& kvs, Vec2 const& value) {
	kvs::set_type(kvs, KVSType::vec2);
	kvs._value.vec2 = value;
}

/// Set 3-dimensional vector value.
inline void vec3(KVS& kvs, Vec3 const& value) {
	kvs::set_type(kvs, KVSType::vec3);
	kvs._value.vec3 = value;
}

/// Set 4-dimensional vector value.
inline void vec4(KVS& kvs, Vec4 const& value) {
	kvs::set_type(kvs, KVSType::vec4);
	kvs._value.vec4 = value;
}

/// Shrink collection capacity to fit current size.
inline void shrink_to_fit(KVS& kvs) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	if (kvs._value.collection.capacity != kvs._value.collection.size) {
		kvs::set_capacity(kvs, kvs._value.collection.size);
	}
}

/// Reserve at least new_capacity in collection.
inline void reserve(KVS& kvs, u32 const new_capacity) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	if (new_capacity > kvs._value.collection.capacity) {
		kvs::set_capacity(kvs, new_capacity);
	}
}

/// Get last item in collection.
inline KVS& back(KVS& kvs) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	TOGO_ASSERTE(kvs::any(kvs));
	return kvs._value.collection.data[kvs._value.collection.size - 1];
}

/// Get last item in collection.
inline KVS& back(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	TOGO_ASSERTE(kvs::any(kvs));
	return kvs._value.collection.data[kvs._value.collection.size - 1];
}

/// Add an item to the end.
inline KVS& push_back(KVS& kvs, KVS const& item) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	kvs::resize(kvs, kvs._value.collection.size + 1);
	KVS& back = kvs::back(kvs);
	kvs::copy(back, item);
	return back;
}

/// Add an item to the end (rvalue reference).
inline KVS& push_back(KVS& kvs, KVS&& item) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	kvs::resize(kvs, kvs._value.collection.size + 1);
	KVS& back = kvs::back(kvs);
	kvs::move(back, item);
	return back;
}

/// Remove the last item.
inline void pop_back(KVS& kvs) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	TOGO_ASSERTE(kvs::any(kvs));
	kvs::resize(kvs, kvs._value.collection.size - 1);
}

/** @} */ // end of doc-group lib_core_kvs

} // namespace kvs

/// This will nullify the KVS.
inline KVS::~KVS() {
	kvs::nullify(*this);
	kvs::clear_name(*this);
}

/// Construct to type with no value initialization.
inline KVS::KVS(KVSType const type, no_init_tag const)
	: _type(type)
	, _name_size(0)
	, _name_hash(KVS_NAME_NULL)
	, _name(nullptr)
{}

/// Construct to type.
inline KVS::KVS(KVSType const type)
	: _type(type)
	, _name_size(0)
	, _name_hash(KVS_NAME_NULL)
	, _name(nullptr)
	, _value()
{}

/// Construct null with no value initialization.
inline KVS::KVS(no_init_tag const)
	: KVS(KVSType::null, no_init_tag{})
{}

/// Construct null.
inline KVS::KVS()
	: KVS(KVSType::null)
{}

inline KVS::KVS(KVS const& other) : KVS(no_init_tag{}) { kvs::copy(*this, other); }

/// Construct with integer value.
inline KVS::KVS(s64 const value) : KVS(KVSType::integer, no_init_tag{}) { _value.integer = value; }

/// Construct with integer value.
inline KVS::KVS(s32 const value) : KVS(KVSType::integer, no_init_tag{}) { _value.integer = value; }

/// Construct with decimal value.
inline KVS::KVS(f64 const value) : KVS(KVSType::decimal, no_init_tag{}) { _value.decimal = value; }

/// Construct with boolean value.
inline KVS::KVS(bool const value, bool_tag const) : KVS(KVSType::boolean, no_init_tag{}) { _value.boolean = value; }

/// Construct with string value.
inline KVS::KVS(StringRef const& value) : KVS(no_init_tag{}) { kvs::string(*this, value); }

/// Construct with 1-dimensional vector value.
inline KVS::KVS(Vec1 const& value) : KVS(KVSType::vec1, no_init_tag{}) { _value.vec1 = value; }

/// Construct with 2-dimensional vector value.
inline KVS::KVS(Vec2 const& value) : KVS(KVSType::vec2, no_init_tag{}) { _value.vec2 = value; }

/// Construct with 3-dimensional vector value.
inline KVS::KVS(Vec3 const& value) : KVS(KVSType::vec3, no_init_tag{}) { _value.vec3 = value; }

/// Construct with 4-dimensional vector value.
inline KVS::KVS(Vec4 const& value) : KVS(KVSType::vec4, no_init_tag{}) { _value.vec4 = value; }

/// Construct with name and null value.
inline KVS::KVS(StringRef const& name, null_tag const) : KVS() { kvs::set_name(*this, name); }

/// Construct with name and type.
inline KVS::KVS(StringRef const& name, KVSType const type) : KVS(type) { kvs::set_name(*this, name); }

/// Construct with name and integer value.
inline KVS::KVS(StringRef const& name, s64 const value) : KVS(value) { kvs::set_name(*this, name); }

/// Construct with name and integer value.
inline KVS::KVS(StringRef const& name, s32 const value) : KVS(value) { kvs::set_name(*this, name); }

/// Construct with name and decimal value.
inline KVS::KVS(StringRef const& name, f64 const value) : KVS(value) { kvs::set_name(*this, name); }

/// Construct with name and boolean value.
inline KVS::KVS(StringRef const& name, bool const value, bool_tag const) : KVS(value, bool_tag{}) { kvs::set_name(*this, name); }

/// Construct with name and string value.
inline KVS::KVS(StringRef const& name, StringRef const& value) : KVS(value) { kvs::set_name(*this, name); }

/// Construct with name and 1-dimensional vector value.
inline KVS::KVS(StringRef const& name, Vec1 const& value) : KVS(value) { kvs::set_name(*this, name); }

/// Construct with name and 2-dimensional vector value.
inline KVS::KVS(StringRef const& name, Vec2 const& value) : KVS(value) { kvs::set_name(*this, name); }

/// Construct with name and 3-dimensional vector value.
inline KVS::KVS(StringRef const& name, Vec3 const& value) : KVS(value) { kvs::set_name(*this, name); }

/// Construct with name and 4-dimensional vector value.
inline KVS::KVS(StringRef const& name, Vec4 const& value) : KVS(value) { kvs::set_name(*this, name); }

/// Value at index.
inline KVS& KVS::operator[](unsigned const i) {
	TOGO_ASSERTE(kvs::is_type_any(*this, type_mask_collection));
	TOGO_DEBUG_ASSERTE(i < _value.collection.size);
	return _value.collection.data[i];
}

/// Value at index.
inline KVS const& KVS::operator[](unsigned const i) const {
	TOGO_ASSERTE(kvs::is_type_any(*this, type_mask_collection));
	TOGO_DEBUG_ASSERTE(i < _value.collection.size);
	return _value.collection.data[i];
}

/** @cond INTERNAL */

// ADL support

inline KVS* begin(KVS& kvs) { return kvs::begin(kvs); }
inline KVS const* begin(KVS const& kvs) { return kvs::begin(kvs); }

inline KVS* end(KVS& kvs) { return kvs::end(kvs); }
inline KVS const* end(KVS const& kvs) { return kvs::end(kvs); }

/** @endcond */ // INTERNAL

} // namespace togo
