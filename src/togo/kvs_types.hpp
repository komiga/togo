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
enum class KVSType : unsigned {
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
	char* _name;
	unsigned _name_size;
	hash64 _name_hash;

	union Value {
		s64 integer;
		f64 decimal;
		bool boolean;
		Vec1 vec1;
		Vec2 vec2;
		Vec3 vec3;
		Vec4 vec4;
		struct {
			char* data;
			unsigned size;
			unsigned capacity;
		} string;
		struct {
			KVS* data;
			unsigned size;
			unsigned capacity;
		} collection;

		Value(Value const&) = delete;
		Value& operator=(Value const&) = delete;

		~Value() = default;
		Value(Value&&) = default;
		Value& operator=(Value&&) = default;

		inline Value()
			: integer(0)
		{}
	} _value;

	KVS& operator=(KVS const&) = delete;
	KVS& operator=(KVS&&) = delete;

	~KVS();

private:
	KVS(KVSType const type);

public:
	KVS();
	KVS(KVS const& other);
	KVS(KVS&& other);

// value ctors
	KVS(s64 const value);
	KVS(f64 const value);
	KVS(bool const value);

	/// Construct with string value.
	KVS(StringRef const& value);
	template<unsigned N>
	KVS(char const (&value)[N]);

	KVS(Vec1 const& value);
	KVS(Vec2 const& value);
	KVS(Vec3 const& value);
	KVS(Vec4 const& value);

// with-name ctors
	KVS(StringRef const& name, null_tag const);

	KVS(StringRef const& name, s64 const value);
	KVS(StringRef const& name, f64 const value);
	KVS(StringRef const& name, bool const value);

	KVS(StringRef const& name, StringRef const& value);
	template<unsigned N>
	KVS(StringRef const& name, char const (&value)[N]);

	KVS(StringRef const& name, Vec1 const& value);
	KVS(StringRef const& name, Vec2 const& value);
	KVS(StringRef const& name, Vec3 const& value);
	KVS(StringRef const& name, Vec4 const& value);

	KVS& operator[](unsigned const i);
	KVS const& operator[](unsigned const i) const;
};

/** @} */ // end of doc-group kvs

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<KVSType> : true_type {};
/** @endcond */ // INTERNAL

} // namespace togo
