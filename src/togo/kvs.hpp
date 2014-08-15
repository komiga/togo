#line 2 "togo/kvs.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file kvs.hpp
@brief KVS interface.
@ingroup kvs
*/

#pragma once

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/kvs_types.hpp>
#include <togo/assert.hpp>

namespace togo {

namespace kvs {

/**
	@addtogroup kvs
	@{
*/

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

/// Get type.
inline KVSType type(KVS const& kvs) {
	return kvs._type;
}

/// Get name.
///
/// This will return nullptr if the KVS has no name.
inline char const* name(KVS const& kvs) {
	return kvs._name;
}

/// Get name size.
inline unsigned name_size(KVS const& kvs) {
	return kvs._name_size;
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
inline bool is_null(KVS const& kvs) {
	return kvs::is_type(kvs, KVSType::null);
}

/// Check if type is KVSType::integer.
inline bool is_integer(KVS const& kvs) {
	return kvs::is_type(kvs, KVSType::integer);
}

/// Check if type is KVSType::decimal.
inline bool is_decimal(KVS const& kvs) {
	return kvs::is_type(kvs, KVSType::decimal);
}

/// Check if type is KVSType::boolean.
inline bool is_boolean(KVS const& kvs) {
	return kvs::is_type(kvs, KVSType::boolean);
}

/// Check if type is KVSType::string.
inline bool is_string(KVS const& kvs) {
	return kvs::is_type(kvs, KVSType::string);
}

/// Check if type is a vector type.
inline bool is_vector(KVS const& kvs) {
	return kvs::is_type_any(kvs, type_mask_vector);
}

/// Check if type is an array or node.
inline bool is_collection(KVS const& kvs) {
	return kvs::is_type_any(kvs, type_mask_collection);
}

/// Check if named.
inline bool is_named(KVS const& kvs) {
	return kvs._name;
}

/// Number of values in the collection.
inline unsigned size(KVS const& kvs) {
	return kvs::is_collection(kvs) ? kvs._value.collection.size : 0;
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
inline f32 decimal(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::decimal));
	return kvs._value.decimal;
}

/// Get boolean value.
inline bool boolean(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::boolean));
	return kvs._value.boolean;
}

/// Get string value.
inline char const* string(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return kvs._value.string.data;
}

/// Get string value size.
inline unsigned string_size(KVS const& kvs) {
	TOGO_ASSERTE(kvs::is_type(kvs, KVSType::string));
	return kvs._value.string.size;
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

/// Clear value.
void clear(KVS& kvs);

/// Clear value (if dynamic) and change type iff type differs.
bool set_type(KVS& kvs, KVSType const type);

/// Clear value (if dynamic) and change type to KVSType::null.
inline void nullify(KVS& kvs) {
	kvs::set_type(kvs, KVSType::null);
}

/// Set integer value.
inline void integer(KVS& kvs, s64 const value) {
	kvs::set_type(kvs, KVSType::integer);
	kvs._value.integer = value;
}

/// Set decimal value.
inline void decimal(KVS& kvs, f32 const value) {
	kvs::set_type(kvs, KVSType::decimal);
	kvs._value.decimal = value;
}

/// Set boolean value.
inline void boolean(KVS& kvs, bool const value) {
	kvs::set_type(kvs, KVSType::boolean);
	kvs._value.boolean = value;
}

/// Set string value.
void string(KVS& kvs, char const* const value, unsigned const size);

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

/** @} */ // end of doc-group kvs

} // namespace kvs

/// This will nullify the KVS.
inline KVS::~KVS() {
	kvs::nullify(*this);
}

/// Construct null KVS.
inline KVS::KVS()
	: _type(KVSType::null)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{}

/// Construct with integer value.
inline KVS::KVS(s64 const value)
	: _type(KVSType::integer)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{
	_value.integer = value;
}

/// Construct with decimal value.
inline KVS::KVS(f32 const value)
	: _type(KVSType::decimal)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{
	_value.decimal = value;
}

/// Construct with boolean value.
inline KVS::KVS(bool const value)
	: _type(KVSType::boolean)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{
	_value.boolean = value;
}

/// Construct with string value.
template<unsigned N>
inline KVS::KVS(char const (&value)[N])
	: KVS(value, N)
{}

/// Construct with 1-dimensional vector value.
inline KVS::KVS(Vec1 const& value)
	: _type(KVSType::vec1)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{
	_value.vec1 = value;
}

/// Construct with 2-dimensional vector value.
inline KVS::KVS(Vec2 const& value)
	: _type(KVSType::vec2)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{
	_value.vec2 = value;
}

/// Construct with 3-dimensional vector value.
inline KVS::KVS(Vec3 const& value)
	: _type(KVSType::vec3)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{
	_value.vec3 = value;
}

/// Construct with 4-dimensional vector value.
inline KVS::KVS(Vec4 const& value)
	: _type(KVSType::vec4)
	, _name(nullptr)
	, _name_size(0)
	, _value()
{
	_value.vec4 = value;
}

/** @cond INTERNAL */

// ADL support

inline KVS* begin(KVS& kvs) { return kvs::begin(kvs); }
inline KVS const* begin(KVS const& kvs) { return kvs::begin(kvs); }

inline KVS* end(KVS& kvs) { return kvs::end(kvs); }
inline KVS const* end(KVS const& kvs) { return kvs::end(kvs); }

/** @endcond */ // INTERNAL

} // namespace togo