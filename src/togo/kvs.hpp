#line 2 "togo/kvs.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief KVS interface.
@ingroup kvs
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/utility.hpp>
#include <togo/string_types.hpp>
#include <togo/kvs_types.hpp>
#include <togo/assert.hpp>
#include <togo/string.hpp>
#include <togo/hash.hpp>
#include <togo/io_types.hpp>

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
	@addtogroup kvs
	@{
*/

/// Get type.
inline KVSType type(KVS const& kvs) {
	return kvs._type;
}

/// Check type equivalence.
inline bool is_type(KVS const& kvs, KVSType const type) {
	return kvs._type == type;
}

/// Check type by set.
inline bool is_type_any(KVS const& kvs, KVSType const type) {
	return enum_bool(kvs._type & type);
}

/// Check if type is KVSType::null.
inline bool is_null(KVS const& kvs) { return kvs::is_type(kvs, KVSType::null); }

/// Check if type is KVSType::integer.
inline bool is_integer(KVS const& kvs) { return kvs::is_type(kvs, KVSType::integer); }

/// Check if type is KVSType::decimal.
inline bool is_decimal(KVS const& kvs) { return kvs::is_type(kvs, KVSType::decimal); }

/// Check if type is KVSType::boolean.
inline bool is_boolean(KVS const& kvs) { return kvs::is_type(kvs, KVSType::boolean); }

/// Check if type is KVSType::string.
inline bool is_string(KVS const& kvs) { return kvs::is_type(kvs, KVSType::string); }

/// Check if type is KVSType::vec1.
inline bool is_vec1(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec1); }

/// Check if type is KVSType::vec2.
inline bool is_vec2(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec2); }

/// Check if type is KVSType::vec3.
inline bool is_vec3(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec3); }

/// Check if type is KVSType::vec4.
inline bool is_vec4(KVS const& kvs) { return kvs::is_type(kvs, KVSType::vec4); }

/// Check if type is KVSType::node.
inline bool is_node(KVS const& kvs) { return kvs::is_type(kvs, KVSType::node); }

/// Check if type is KVSType::array.
inline bool is_array(KVS const& kvs) { return kvs::is_type(kvs, KVSType::array); }

/// Check if type is a vector type.
inline bool is_vector(KVS const& kvs) { return kvs::is_type_any(kvs, type_mask_vector); }

/// Check if type is an array or node.
inline bool is_collection(KVS const& kvs) { return kvs::is_type_any(kvs, type_mask_collection); }

/// Get name (NUL-terminated).
inline char const* name(KVS const& kvs) {
	return kvs._name ? kvs._name : "";
}

/// Get name size.
///
/// This does not include the NUL terminator.
inline u32 name_size(KVS const& kvs) {
	return kvs._name_size;
}

/// Get name reference.
inline StringRef name_ref(KVS const& kvs) {
	return StringRef{kvs._name, kvs._name_size};
}

/// Get name hash.
inline hash32 name_hash(KVS const& kvs) {
	return kvs._name_hash;
}

/// Check if named.
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

/// Returns true if there are any values in the collection.
inline bool any(KVS const& kvs) {
	return kvs::is_collection(kvs) ? kvs._value.collection.size : false;
}

/// Returns true if there are no values in the collection.
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

/// Get integer value.
inline s64 integer(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::integer));
	return kvs._value.integer;
}

/// Get decimal value.
inline f64 decimal(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::decimal));
	return kvs._value.decimal;
}

/// Get boolean value.
inline bool boolean(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::boolean));
	return kvs._value.boolean;
}

/// Get string value (NUL-terminated).
inline char const* string(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return kvs._value.string.data ? kvs._value.string.data : "";
}

/// Get string value size.
///
/// This does not include the NUL terminator.
inline u32 string_size(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return kvs._value.string.size;
}

/// Get string value reference.
inline StringRef string_ref(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return StringRef{
		kvs._value.string.data ? kvs._value.string.data : "",
		kvs._value.string.size
	};
}

/// Get 1-dimensional vector value.
inline Vec1 const& vec1(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec1));
	return kvs._value.vec1;
}

/// Get 2-dimensional vector value.
inline Vec2 const& vec2(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec2));
	return kvs._value.vec2;
}

/// Get 3-dimensional vector value.
inline Vec3 const& vec3(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec3));
	return kvs._value.vec3;
}

/// Get 4-dimensional vector value.
inline Vec4 const& vec4(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::vec4));
	return kvs._value.vec4;
}

/// Find item in collection by name.
KVS* find(KVS& kvs, StringRef const& name);

/// Find item in collection by name.
KVS const* find(KVS const& kvs, StringRef const& name);

/// Find item in collection by name hash.
KVS* find(KVS& kvs, hash32 name_hash);

/// Find item in collection by name hash.
KVS const* find(KVS const& kvs, hash32 name_hash);

/// Free value if dynamic and change type iff type differs.
///
/// Returns true if the type changed.
bool set_type(KVS& kvs, KVSType const type);

/// Set name.
void set_name(KVS& kvs, StringRef const& name);

/// Clear name.
void clear_name(KVS& kvs);

/// Clear value.
void clear(KVS& kvs);

/// Free dynamic value.
void free_dynamic(KVS& kvs);

/// Copy value.
void copy(KVS& dst, KVS const& src);

/// Move state.
///
/// src will be null and unnamed after this call.
void move(KVS& dst, KVS& src);

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

/// Set string value.
void string(KVS& kvs, StringRef const& value);

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

/// Change collection capacity.
///
/// If new_capacity is lower than the size, the collection is resized
/// to new_capacity.
void set_capacity(KVS& kvs, u32 const new_capacity);

/// Grow collection with a doubling factor.
///
/// Cost of insertion should be amortized O(1), assuming no aggressive
/// shrinking. Grows to at least min_capacity if it is non-zero.
void grow(KVS& kvs, u32 const min_capacity = 0);

/// Shrink collection capacity to fit current size.
inline void shrink_to_fit(KVS& kvs) {
	TOGO_ASSERTE(kvs::is_type_any(kvs, type_mask_collection));
	if (kvs._value.collection.capacity != kvs._value.collection.size) {
		kvs::set_capacity(kvs, kvs._value.collection.size);
	}
}

/// Change collection size.
///
/// Upsize grows by using new_size as the minimum capacity.
/// If the collection grows, the new values will be null.
void resize(KVS& kvs, u32 const new_size);

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

/// Read from stream.
///
/// Returns false if a parser error occurred. pinfo will have the
/// position and error message of the parser.
bool read(KVS& root, IReader& stream, ParserInfo& pinfo);

/// Write to stream.
///
/// Returns IO status of the stream (output is likely incomplete
/// if !status).
IOStatus write(KVS const& kvs, IWriter& stream);

/** @} */ // end of doc-group kvs

} // namespace kvs

/// This will nullify the KVS.
inline KVS::~KVS() {
	kvs::nullify(*this);
	kvs::clear_name(*this);
}

/// Construct to type.
inline KVS::KVS(KVSType const type)
	: _type(type)
	, _name_size(0)
	, _name_hash(hash::IDENTITY32)
	, _name(nullptr)
	, _value()
{}

/// Construct null.
inline KVS::KVS()
	: KVS(KVSType::null)
{}

inline KVS::KVS(KVS const& other) : KVS() { kvs::copy(*this, other); }

/// Construct with integer value.
inline KVS::KVS(s64 const value) : KVS(KVSType::integer) { _value.integer = value; }

/// Construct with integer value.
inline KVS::KVS(s32 const value) : KVS(KVSType::integer) { _value.integer = value; }

/// Construct with decimal value.
inline KVS::KVS(f64 const value) : KVS(KVSType::decimal) { _value.decimal = value; }

/// Construct with boolean value.
inline KVS::KVS(bool const value, bool_tag const) : KVS(KVSType::boolean) { _value.boolean = value; }

/// Construct with string value.
inline KVS::KVS(StringRef const& value) : KVS() { kvs::string(*this, value); }

/// Construct with 1-dimensional vector value.
inline KVS::KVS(Vec1 const& value) : KVS(KVSType::vec1) { _value.vec1 = value; }

/// Construct with 2-dimensional vector value.
inline KVS::KVS(Vec2 const& value) : KVS(KVSType::vec2) { _value.vec2 = value; }

/// Construct with 3-dimensional vector value.
inline KVS::KVS(Vec3 const& value) : KVS(KVSType::vec3) { _value.vec3 = value; }

/// Construct with 4-dimensional vector value.
inline KVS::KVS(Vec4 const& value) : KVS(KVSType::vec4) { _value.vec4 = value; }

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

/// Access value by index.
inline KVS& KVS::operator[](unsigned const i) {
	TOGO_ASSERTE(kvs::is_type_any(*this, type_mask_collection));
	TOGO_DEBUG_ASSERTE(i < _value.collection.size);
	return _value.collection.data[i];
}

/// Access value by index.
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
