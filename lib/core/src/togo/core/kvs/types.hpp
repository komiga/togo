#line 2 "togo/core/kvs/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief KVS types.
@ingroup lib_core_types
@ingroup lib_core_kvs
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/tags.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/math/types.hpp>
#include <togo/core/string/types.hpp>
#include <togo/core/hash/hash.hpp>

namespace togo {

/**
	@addtogroup lib_core_kvs
	@{
*/

/// KVS name hash.
using KVSNameHash = hash32;

/// KVS name hash literal.
inline constexpr KVSNameHash
operator"" _kvs_name(
	char const* const data,
	std::size_t const size
) {
	return hash::calc32_ce(data, size);
}

/// KVS names.
enum : KVSNameHash {
	KVS_NAME_NULL = ""_kvs_name,
};

/// Key-value store type.
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
	/// Ordered collection of named values (non-unique).
	node		= 1 << 10,
};

/// Key-value store.
struct KVS {
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
	};

	KVSType _type;
	u32 _name_size;
	KVSNameHash _name_hash;
	char* _name;
	Value _value;

	KVS& operator=(KVS const&) = delete;
	KVS& operator=(KVS&&) = delete;

	~KVS();

	KVS();
	KVS(no_init_tag const);
	KVS(KVS const& other);
	KVS(KVS&& other);

	KVS(KVSType const type, no_init_tag const);
	KVS(KVSType const type);

// value ctors
	explicit KVS(s64 const value);
	explicit KVS(s32 const value);
	explicit KVS(f64 const value);
	explicit KVS(bool const value, bool_tag const);

	explicit KVS(StringRef const& value);

	explicit KVS(Vec1 const& value);
	explicit KVS(Vec2 const& value);
	explicit KVS(Vec3 const& value);
	explicit KVS(Vec4 const& value);

// with-name ctors
	KVS(StringRef const& name, null_tag const);
	KVS(StringRef const& name, KVSType const type);

	KVS(StringRef const& name, s64 const value);
	KVS(StringRef const& name, s32 const value);
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

/// KVS parser information.
struct ParserInfo {
	/// Line in stream.
	unsigned line{0};
	/// Column on line.
	unsigned column{0};
	/// Error message.
	char message[512]{'\0'};
};

/** @} */ // end of doc-group lib_core_kvs

/** @cond INTERNAL */
template<>
struct enable_enum_bitwise_ops<KVSType> : true_type {};
/** @endcond */ // INTERNAL

} // namespace togo
