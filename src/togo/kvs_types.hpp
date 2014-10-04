#line 2 "togo/kvs_types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file kvs_types.hpp
@brief KVS types.
@ingroup types
@ingroup kvs
*/

#pragma once

#include <togo/config.hpp>
#include <togo/tags.hpp>
#include <togo/traits.hpp>
#include <togo/types.hpp>
#include <togo/math_types.hpp>
#include <togo/string_types.hpp>

namespace togo {

/**
	@addtogroup kvs
	@{
*/

/**
	Key-value store type.
*/
enum class KVSType : u32 {
	/// Empty value.
	null		= 1 << 0,
	/// 64-bit signed integer.
	integer		= 1 << 1,
	/// 64-bit floating-point number.
	decimal		= 1 << 2,
	/// Boolean.
	boolean		= 1 << 3,
	/// String.
	string		= 1 << 4,
	/// 1-dimensional floating-point vector.
	vec1		= 1 << 5,
	/// 2-dimensional floating-point vector.
	vec2		= 1 << 6,
	/// 3-dimensional floating-point vector.
	vec3		= 1 << 7,
	/// 4-dimensional floating-point vector.
	vec4		= 1 << 8,
	/// Ordered collection of unnamed values.
	array		= 1 << 9,
	/// Collection of named values.
	node		= 1 << 10,
};

/**
	Key-value store.
*/
struct KVS {
	KVSType _type;
	u32 _name_size;
	hash64 _name_hash;
	char* _name;

	struct StringValue {
		u32 size;
		u32 capacity;
		char* data;
	};

	struct CollectionValue {
		u32 size;
		u32 capacity;
		KVS* data;
	};

	union Value {
		u8 _init[max(sizeof(Vec4), sizeof(StringValue))];
		s64 integer;
		f64 decimal;
		bool boolean;
		Vec1 vec1;
		Vec2 vec2;
		Vec3 vec3;
		Vec4 vec4;
		StringValue string;
		CollectionValue collection;

		Value(Value const&) = delete;
		Value& operator=(Value const&) = delete;

		~Value() = default;
		Value(Value&&) = default;
		Value& operator=(Value&&) = default;

		inline Value()
			: _init()
		{}
	} _value;

	KVS& operator=(KVS const&) = delete;
	KVS& operator=(KVS&&) = delete;

	~KVS();

public:
	KVS();
	KVS(KVS const& other);
	KVS(KVS&& other);

	KVS(KVSType const type);

// value ctors
	KVS(s64 const value);
	KVS(f64 const value);
	KVS(bool const value, bool_tag const);

	KVS(StringRef const& value);

	KVS(Vec1 const& value);
	KVS(Vec2 const& value);
	KVS(Vec3 const& value);
	KVS(Vec4 const& value);

// with-name ctors
	KVS(StringRef const& name, null_tag const);
	KVS(StringRef const& name, KVSType const type);

	KVS(StringRef const& name, s64 const value);
	KVS(StringRef const& name, f64 const value);
	KVS(StringRef const& name, bool const value, bool_tag const);

	KVS(StringRef const& name, StringRef const& value);

	KVS(StringRef const& name, Vec1 const& value);
	KVS(StringRef const& name, Vec2 const& value);
	KVS(StringRef const& name, Vec3 const& value);
	KVS(StringRef const& name, Vec4 const& value);

	KVS& operator[](unsigned const i);
	KVS const& operator[](unsigned const i) const;
};

/**
	KVS parser information.
*/
struct ParserInfo {
	/// Line in stream.
	unsigned line{0};
	/// Column on line.
	unsigned column{0};
	/// Error message.
	char message[512]{'\0'};
};

/** @} */ // end of doc-group kvs

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<KVSType> : true_type {};
/** @endcond */ // INTERNAL

} // namespace togo
