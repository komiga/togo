#line 2 "togo/object_buffer_type.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Object buffer type.
@ingroup types
@ingroup io
@ingroup object_buffer
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/memory_types.hpp>
#include <togo/memory_io.hpp>

namespace togo {

/**
	@addtogroup object_buffer
	@{
*/

/**
	Object buffer.

	@tparam T Integral arithmetic type to use for object types.
	@tparam S Integral arithmetic type to use for object sizes.
*/
template<class T, class S>
struct ObjectBuffer {
	TOGO_CONSTRAIN_INTEGRAL_ARITHMETIC(T);
	TOGO_CONSTRAIN_INTEGRAL_ARITHMETIC(S);

	MemoryStream _stream;
	unsigned _num_objects;
	bool _consume_mode;

	~ObjectBuffer() = default;

	ObjectBuffer() = delete;
	ObjectBuffer(ObjectBuffer const&) = delete;
	ObjectBuffer(ObjectBuffer&&) = delete;
	ObjectBuffer& operator=(ObjectBuffer const&) = delete;
	ObjectBuffer& operator=(ObjectBuffer&&) = delete;

	ObjectBuffer(Allocator& allocator, u32 const init_capacity);
};

/** @} */ // end of doc-group object_buffer

} // namespace togo
